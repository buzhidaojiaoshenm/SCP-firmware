/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "internal/fmu_reg.h"

#include <mod_fmu.h>

#include <fwk_assert.h>
#include <fwk_id.h>
#include <fwk_interrupt.h>
#include <fwk_log.h>
#include <fwk_math.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_notification.h>
#include <fwk_status.h>

#define MOD_NAME "[FMU] "

#define LSB_GET(value) ((value) & -(value))

#define FMU_ROOT_IDX       0
#define FMU_MAX_TREE_DEPTH 4

static struct {
    const struct mod_fmu_dev_config **device_config;
    unsigned int num_devices;
} ctx;

static unsigned int find_next_fmu(
    unsigned int device_idx,
    unsigned int node_idx)
{
    unsigned int idx;
    const struct mod_fmu_dev_config *device_config;

    if (device_idx == MOD_FMU_PARENT_NONE || node_idx == MOD_FMU_PARENT_NONE) {
        return MOD_FMU_PARENT_NONE;
    }

    for (idx = 0; idx < ctx.num_devices; idx++) {
        device_config = ctx.device_config[idx];
        if (device_config->parent == device_idx &&
            (device_config->parent_cr_index == node_idx ||
             device_config->parent_ncr_index == node_idx)) {
            return idx;
        }
    }

    return MOD_FMU_PARENT_NONE;
}

static unsigned int find_active_node(unsigned int device_idx)
{
    uint64_t errgsr;
    unsigned int errgsr_idx;
    const struct mod_fmu_dev_config *config;

    if (device_idx == MOD_FMU_PARENT_NONE) {
        return MOD_FMU_PARENT_NONE;
    }

    config = ctx.device_config[device_idx];

    /* Determine fault record idx */
    for (errgsr_idx = 0; errgsr_idx <= FMU_ERRGSR_MAX; errgsr_idx++) {
        errgsr = fmu_read_32(config->base, FMU_FIELD_ERRGSR_L(errgsr_idx)) |
            ((uint64_t)fmu_read_32(config->base, FMU_FIELD_ERRGSR_H(errgsr_idx))
             << FMU_ERRGSR_NUM_BITS);

        if (errgsr != 0) {
            return (errgsr_idx * FMU_ERRGSR_NUM_BITS * 2) +
                fwk_math_log2(LSB_GET(errgsr));
        }
    }

    return MOD_FMU_PARENT_NONE;
}

static bool next_fault(bool critical)
{
    unsigned int notifications_sent, depth;
    unsigned int device_idx = MOD_FMU_PARENT_NONE, next_device_idx;
    unsigned int node_idx = MOD_FMU_PARENT_NONE, next_node_idx;
    uint8_t sm_idx;
    const struct mod_fmu_dev_config *config;
    uint32_t val;
#ifdef BUILD_HAS_NOTIFICATION
    struct mod_fmu_fault_notification_params *params;
#endif

    /* Traverse tree until no device is found for fault record */
    for (next_device_idx = FMU_ROOT_IDX, depth = 0;
         next_device_idx != MOD_FMU_PARENT_NONE;
         next_device_idx = find_next_fmu(next_device_idx, next_node_idx),
        depth++) {
        if (depth >= FMU_MAX_TREE_DEPTH) {
            FWK_LOG_ERR(MOD_NAME "Maximum tree depth reached");
            fwk_trap();
        }

        next_node_idx = find_active_node(next_device_idx);
        /* If current FMU has an active fault record, select and acknowledge it
         */
        if (next_node_idx != MOD_FMU_PARENT_NONE) {
            node_idx = next_node_idx;
            device_idx = next_device_idx;

            /* Acknowledge the fault */
            config = ctx.device_config[device_idx];
            val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_idx));
            val |= FMU_ERRIMPDEF_IC_MASK;
            fmu_write_32(config->base, FMU_FIELD_ERRIMPDEF(node_idx), val);
        }
    }

    if (device_idx == MOD_FMU_PARENT_NONE || node_idx == MOD_FMU_PARENT_NONE) {
        return false;
    }

    /* Collect data, log details and raise event */
    config = ctx.device_config[device_idx];
    sm_idx = (fmu_read_32(config->base, FMU_FIELD_ERR_STATUS(node_idx)) &
              FMU_ERR_STATUS_IERR_MASK) >>
        FMU_ERR_STATUS_IERR_SHIFT;

    FWK_LOG_INFO(
        MOD_NAME "%s fault received: Device: 0x%x, Node 0x%x, SM 0x%x",
        critical ? "Critical" : "Non-critical",
        device_idx,
        node_idx,
        sm_idx);

#ifdef BUILD_HAS_NOTIFICATION
    struct fwk_event event = {
        .id = mod_fmu_notification_id_fault,
        .source_id = FWK_ID_MODULE_INIT(FWK_MODULE_IDX_FMU),
    };
    params = (struct mod_fmu_fault_notification_params *)event.params;
    params->critical = critical;
    params->fault.device_idx = device_idx;
    params->fault.node_idx = node_idx;
    params->fault.sm_idx = sm_idx;

    if (fwk_notification_notify(&event, &notifications_sent) != FWK_SUCCESS) {
        FWK_LOG_ERR(MOD_NAME "Error raising notification");
        fwk_trap();
    }
#endif

    return true;
}

/*
 * Common interrupt handler
 */
static void fmu_isr(bool critical)
{
    unsigned int i;

    /* Discover all active fault records */
    for (i = 0; next_fault(critical); i++) {
        continue;
    }

    if (i == 0) {
        FWK_LOG_ERR(MOD_NAME "No error record found");
        fwk_trap();
    }
}

/*
 * Critical fault ISR
 */
static void fmu_isr_critical(void)
{
    fmu_isr(true);
}

/*
 * Non-critical fault ISR
 */
static void fmu_isr_non_critical(void)
{
    fmu_isr(false);
}

/*
 * API Handlers
 */
static int inject(const struct mod_fmu_fault *fault)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fault == NULL || fault->device_idx >= ctx.num_devices) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fault->device_idx];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(fault->node_idx));
    val |= ((uint32_t)fault->sm_idx << FMU_ERRIMPDEF_IE_SHIFT) &
        FMU_ERRIMPDEF_IE_MASK;
    /* Ensure injection ack bits are cleared */
    val &= ~(FMU_ERRIMPDEF_IC_MASK);

    fmu_write_32(config->base, FMU_FIELD_ERRIMPDEF(fault->node_idx), val);

    return FWK_SUCCESS;
}

static int get_enabled(fwk_id_t id, uint16_t node_id, bool *enabled)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices || enabled == NULL) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERR_CTRL(node_id));
    *enabled = (val & FMU_ERR_CTRL_ED_MASK) != 0;

    return FWK_SUCCESS;
}

static int set_enabled(fwk_id_t id, uint16_t node_id, bool enabled)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERR_CTRL(node_id));
    if (enabled) {
        val |= FMU_ERR_CTRL_ENABLE_MASK;
    } else {
        val &= ~FMU_ERR_CTRL_ENABLE_MASK;
    }
    fmu_write_32(config->base, FMU_FIELD_ERR_CTRL(node_id), val);

    return FWK_SUCCESS;
}

static int get_count(fwk_id_t id, uint16_t node_id, uint8_t *count)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices || count == NULL) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_id));
    *count = (val & FMU_ERRIMPDEF_CNT_MASK) >> FMU_ERRIMPDEF_CNT_SHIFT;

    return FWK_SUCCESS;
}

static int set_count(fwk_id_t id, uint16_t node_id, uint8_t count)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_id));
    val &= ~FMU_ERRIMPDEF_CNT_MASK;
    val |= ((uint32_t)count << FMU_ERRIMPDEF_CNT_SHIFT);
    fmu_write_32(config->base, FMU_FIELD_ERRIMPDEF(node_id), val);

    return FWK_SUCCESS;
}

static int get_threshold(fwk_id_t id, uint16_t node_id, uint8_t *threshold)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices || threshold == NULL) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_id));
    *threshold = (val & FMU_ERRIMPDEF_THR_MASK) >> FMU_ERRIMPDEF_THR_SHIFT;

    return FWK_SUCCESS;
}

static int set_threshold(fwk_id_t id, uint16_t node_id, uint8_t threshold)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_id));
    val &= ~FMU_ERRIMPDEF_THR_MASK;
    val |= ((uint32_t)threshold << FMU_ERRIMPDEF_THR_SHIFT);
    fmu_write_32(config->base, FMU_FIELD_ERRIMPDEF(node_id), val);

    return FWK_SUCCESS;
}

static int get_upgrade_enabled(fwk_id_t id, uint16_t node_id, bool *enabled)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices || enabled == NULL) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_id));
    *enabled = (val & FMU_ERRIMPDEF_UE_MASK) != 0;

    return FWK_SUCCESS;
}

static int set_upgrade_enabled(fwk_id_t id, uint16_t node_id, bool enabled)
{
    uint32_t val;
    const struct mod_fmu_dev_config *config;

    if (fwk_id_get_element_idx(id) >= ctx.num_devices) {
        return FWK_E_PARAM;
    }

    config = ctx.device_config[fwk_id_get_element_idx(id)];

    val = fmu_read_32(config->base, FMU_FIELD_ERRIMPDEF(node_id));
    if (enabled) {
        val |= FMU_ERRIMPDEF_UE_MASK;
    } else {
        val &= ~FMU_ERRIMPDEF_UE_MASK;
    }
    fmu_write_32(config->base, FMU_FIELD_ERRIMPDEF(node_id), val);

    return FWK_SUCCESS;
}

static struct mod_fmu_api fmu_api = {
    .inject = inject,
    .get_enabled = get_enabled,
    .set_enabled = set_enabled,
    .get_count = get_count,
    .set_count = set_count,
    .get_threshold = get_threshold,
    .set_threshold = set_threshold,
    .get_upgrade_enabled = get_upgrade_enabled,
    .set_upgrade_enabled = set_upgrade_enabled,
};

/*
 * Framework Handlers
 */
static int fmu_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    fwk_assert(data != NULL);
    fwk_assert(element_count > 0);

    ctx.num_devices = element_count;
    ctx.device_config =
        fwk_mm_calloc(element_count, sizeof(struct mod_fmu_dev_config *));

    return FWK_SUCCESS;
}

static int fmu_device_init(
    fwk_id_t device_id,
    unsigned int unused,
    const void *data)
{
    fwk_assert(data != NULL);
    fwk_assert(fwk_id_get_element_idx(device_id) < ctx.num_devices);

    ctx.device_config[fwk_id_get_element_idx(device_id)] = data;

    return FWK_SUCCESS;
}

static int fmu_start(fwk_id_t id)
{
    int status;
    const struct mod_fmu_config *config = fwk_module_get_data(id);

    if (!fwk_id_is_type(id, FWK_ID_TYPE_MODULE)) {
        return FWK_SUCCESS;
    }

    status = fwk_interrupt_set_isr(config->irq_critical, &fmu_isr_critical);
    if (status != FWK_SUCCESS) {
        return status;
    }

    status =
        fwk_interrupt_set_isr(config->irq_non_critical, &fmu_isr_non_critical);
    if (status != FWK_SUCCESS) {
        return status;
    }

    status = fwk_interrupt_enable(config->irq_critical);
    if (status != FWK_SUCCESS) {
        return status;
    }

    status = fwk_interrupt_enable(config->irq_non_critical);
    if (status != FWK_SUCCESS) {
        return status;
    }

    return FWK_SUCCESS;
}

static int fmu_process_bind_request(
    fwk_id_t requester_id,
    fwk_id_t target_id,
    fwk_id_t api_id,
    const void **api)
{
    switch (fwk_id_get_api_idx(api_id)) {
    case MOD_FMU_DEVICE_API_IDX:
        *api = &fmu_api;
        break;
    default:
        return FWK_E_PARAM;
    }
    return FWK_SUCCESS;
}

const struct fwk_module module_fmu = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = fmu_init,
    .element_init = fmu_device_init,
    .start = fmu_start,
    .api_count = MOD_FMU_API_COUNT,
    .process_bind_request = fmu_process_bind_request,
#ifdef BUILD_HAS_NOTIFICATION
    .notification_count = (unsigned int)MOD_FMU_NOTIFICATION_IDX_COUNT,
#endif
};
