/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, STMicroelectronics and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <scmi_agents.h>

#include <mod_optee_mbx.h>
#include <mod_scmi.h>

#include <fwk_assert.h>
#include <fwk_element.h>
#include <fwk_id.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

#include <arch_main.h>

#ifdef CFG_SCPFW_MOD_MSG_SMT
#    include <mod_msg_smt.h>
#endif

#include <kernel/panic.h>
#include <scmi_agent_configuration.h>
#include <util.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* SCMI agent and services (channels) */
static struct mod_scmi_agent *scmi_agent_table;
static struct mod_scmi_config scmi_data;
static struct fwk_element *scmi_service_elt;

/* SCMI channel mailbox/shmem */
#ifdef CFG_SCPFW_MOD_MSG_SMT
static struct fwk_element *msg_smt_elt;
static struct mod_msg_smt_channel_config *msg_smt_data;
#endif
static struct fwk_element *optee_mbx_elt;
static struct mod_optee_mbx_channel_config *optee_mbx_data;

/* Config data for scmi module */
static const struct fwk_element *get_scmi_service_table(fwk_id_t module_id)
{
    return scmi_service_elt; /* scmi_service_elt filled during initialization */
}

struct fwk_module_config config_scmi = {
    .data = (void *)&scmi_data, /* scmi_data filled during initialization */
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(get_scmi_service_table),
};

/* Config data for optee_mbx module */
static const struct fwk_element *optee_mbx_get_element_table(fwk_id_t module_id)
{
    return (const struct fwk_element *)optee_mbx_elt;
}

struct fwk_module_config config_optee_mbx = {
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(optee_mbx_get_element_table),
};

#ifdef CFG_SCPFW_MOD_MSG_SMT
/* Config data for msg_smt module */
static const struct fwk_element *msg_smt_get_element_table(fwk_id_t module_id)
{
    fwk_assert(fwk_id_get_module_idx(module_id) == FWK_MODULE_IDX_MSG_SMT);
    return (const struct fwk_element *)msg_smt_elt;
}

struct fwk_module_config config_msg_smt = {
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(msg_smt_get_element_table),
};
#endif

/*
 * Indices state when applying agents configuration
 * @channel_count: Number of channels (mailbox/shmem links) used
 */
struct scpfw_resource_counter {
    size_t channel_count;
} scpfw_resource_counter;

/*
 * Count once for all the several instances and allocate global resources.
 * Global resources are clock, optee/clock, reset, optee/reset, regu,
 * optee/regu, psu, optee/psu, dvfs, perfd, ...;
 */
static void count_resources(struct scpfw_config *cfg)
{
    for (size_t i = 0; i < cfg->agent_count; i++) {
        struct scpfw_agent_config *agent_cfg = cfg->agent_config + i;

        scpfw_resource_counter.channel_count += agent_cfg->channel_count;
    }
}

/*
 * Allocate all tables that may be needed. An optimized implementation would
 * allocate a single piece of memory and set the pointers accordingly.
 */
static void allocate_global_resources(struct scpfw_config *cfg)
{
    size_t __maybe_unused scmi_agent_count;

    /*
     * @cfg does not consider agent #0,
     * that is the reserved platform/server agent.
     */
    scmi_agent_count = cfg->agent_count + 1;
}

enum mailbox_type {
    MAILBOX_TYPE_MSG_SMT,
    MAILBOX_TYPE_COUNT,
};

static enum mailbox_type module_idx_smt_table[SCMI_AGENT_ID_COUNT] = {
    /*
     * Agent NSEC0 is Cortex-A that communicates over OP-TEE's SCMI PTA
     * and OP-TEE shared memory hence using MSG_SMT module.
     */
    [SCMI_AGENT_ID_NSEC0] = MAILBOX_TYPE_MSG_SMT,
};

static void set_scmi_comm_resources(struct scpfw_config *cfg)
{
    unsigned int channel_index = 0;
    unsigned int __maybe_unused msg_smt_index = 0;
    size_t scmi_agent_count = cfg->agent_count;

    /*
     * @cfg does not consider agent #0,
     * that is the reserved platform/server agent.
     */
    scmi_agent_table =
        fwk_mm_calloc(scmi_agent_count + 1, sizeof(*scmi_agent_table));

    scmi_service_elt = fwk_mm_calloc(
        scpfw_resource_counter.channel_count + 1, sizeof(*scmi_service_elt));

#ifdef CFG_SCPFW_MOD_MSG_SMT
    msg_smt_elt = fwk_mm_calloc(
        scpfw_resource_counter.channel_count + 1, sizeof(*msg_smt_elt));
    msg_smt_data = fwk_mm_calloc(
        scpfw_resource_counter.channel_count, sizeof(*msg_smt_data));
#endif

    optee_mbx_elt = fwk_mm_calloc(
        scpfw_resource_counter.channel_count + 1, sizeof(*optee_mbx_elt));
    optee_mbx_data = fwk_mm_calloc(
        scpfw_resource_counter.channel_count, sizeof(*optee_mbx_data));

    /* Set now the uniqnue scmi module instance configuration data */
    scmi_data = (struct mod_scmi_config){
        .agent_table = scmi_agent_table,
        .agent_count = scmi_agent_count,
        .protocol_count_max = 9,
        .vendor_identifier = "ST",
        .sub_vendor_identifier = "ST",
    };

    for (size_t i = 0; i < cfg->agent_count; i++) {
        struct scpfw_agent_config *agent_cfg = cfg->agent_config + i;
        size_t agent_index = agent_cfg->agent_id;

        scmi_agent_table[agent_index].type = SCMI_AGENT_TYPE_OSPM;
        scmi_agent_table[agent_index].name = agent_cfg->name;

        for (size_t j = 0; j < agent_cfg->channel_count; j++) {
            struct scpfw_channel_config *channel_cfg =
                agent_cfg->channel_config + j;
            struct mod_scmi_service_config *service_data;

            service_data = fwk_mm_calloc(1, sizeof(*service_data));

            scmi_service_elt[channel_index].name = channel_cfg->name;
            scmi_service_elt[channel_index].data = service_data;

            optee_mbx_elt[channel_index].name = channel_cfg->name;
            optee_mbx_elt[channel_index].data =
                (void *)(optee_mbx_data + channel_index);

            assert(agent_index < SCMI_AGENT_ID_COUNT);

            switch (module_idx_smt_table[agent_index]) {
#if defined(CFG_SCPFW_MOD_MSG_SMT)
            case MAILBOX_TYPE_MSG_SMT:
                *service_data = (struct mod_scmi_service_config){
                    .transport_id = (fwk_id_t)FWK_ID_ELEMENT_INIT(
                        FWK_MODULE_IDX_MSG_SMT, msg_smt_index),
                    .transport_api_id = (fwk_id_t)FWK_ID_API_INIT(
                        FWK_MODULE_IDX_MSG_SMT,
                        MOD_MSG_SMT_API_IDX_SCMI_TRANSPORT),
                    .scmi_agent_id = agent_cfg->agent_id,
                    .scmi_p2a_id = FWK_ID_NONE_INIT,
                };

                msg_smt_elt[msg_smt_index].name = channel_cfg->name;
                msg_smt_elt[msg_smt_index].data =
                    (void *)(msg_smt_data + msg_smt_index);

                msg_smt_data[msg_smt_index] =
                    (struct mod_msg_smt_channel_config){
                        .type = MOD_MSG_SMT_CHANNEL_TYPE_REQUESTER,
                        .mailbox_size = SCMI_SHMEM_SIZE,
                        .driver_id = (fwk_id_t)FWK_ID_ELEMENT_INIT(
                            FWK_MODULE_IDX_OPTEE_MBX, channel_index),
                        .driver_api_id = (fwk_id_t)FWK_ID_API_INIT(
                            FWK_MODULE_IDX_OPTEE_MBX, 0),
                    };

                optee_mbx_data[channel_index] =
                    (struct mod_optee_mbx_channel_config){
                        .driver_id = (fwk_id_t)FWK_ID_ELEMENT_INIT(
                            FWK_MODULE_IDX_MSG_SMT, msg_smt_index),
                        .driver_api_id = (fwk_id_t)FWK_ID_API_INIT(
                            FWK_MODULE_IDX_MSG_SMT,
                            MOD_MSG_SMT_API_IDX_DRIVER_INPUT),
                    };
                msg_smt_index++;
                break;
#endif
            default:
                panic("Incorrect SMT module_idx");
            }

            channel_index++;
        }
    }
};

static void set_resources(struct scpfw_config *cfg)
{
    for (size_t i = 0; i < cfg->agent_count; i++) {
        struct scpfw_agent_config *agent_cfg = cfg->agent_config + i;
        size_t agent_index = i + 1;

        if (agent_index != agent_cfg->agent_id) {
            panic("scpfw config expects agent ID is agent index");
        }
    }
}

int scmi_configure(struct scpfw_config *cfg)
{
    count_resources(cfg);
    allocate_global_resources(cfg);
    set_scmi_comm_resources(cfg);
    set_resources(cfg);

    return 0;
}
