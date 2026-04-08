/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_scmi.h>
#include <mod_transport.h>

#include <fwk_element.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

enum qemu_virt_m7_scmi_service_idx {
    QEMU_VIRT_M7_SCMI_SERVICE_IDX_AP,
    QEMU_VIRT_M7_SCMI_SERVICE_IDX_COUNT,
};

static const struct fwk_element service_table[] = {
    [QEMU_VIRT_M7_SCMI_SERVICE_IDX_AP] = {
        .name = "AP",
        .data = &((struct mod_scmi_service_config) {
            .transport_id = FWK_ID_ELEMENT_INIT(
                FWK_MODULE_IDX_TRANSPORT,
                QEMU_VIRT_M7_SCMI_SERVICE_IDX_AP),
            .transport_api_id = FWK_ID_API_INIT(
                FWK_MODULE_IDX_TRANSPORT,
                MOD_TRANSPORT_API_IDX_SCMI_TO_TRANSPORT),
            .transport_notification_init_id = FWK_ID_NONE_INIT,
            .scmi_agent_id = 1,
            .scmi_p2a_id = FWK_ID_NONE_INIT,
        }),
    },
    [QEMU_VIRT_M7_SCMI_SERVICE_IDX_COUNT] = { 0 },
};

static const struct fwk_element *get_service_table(fwk_id_t module_id)
{
    (void)module_id;

    return service_table;
}

static const struct mod_scmi_agent agent_table[] = {
    [1] = {
        .type = SCMI_AGENT_TYPE_OSPM,
        .name = "TF-A",
    },
};

struct fwk_module_config config_scmi = {
    .data = &(struct mod_scmi_config) {
        .protocol_count_max = 1,
        .agent_count = 1,
        .agent_table = agent_table,
        .vendor_identifier = "qemu",
        .sub_vendor_identifier = "virt",
    },
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(get_service_table),
};
