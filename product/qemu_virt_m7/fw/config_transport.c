/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_transport.h>

#include <fwk_element.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

enum qemu_virt_m7_transport_idx {
    QEMU_VIRT_M7_TRANSPORT_IDX_SCMI,
    QEMU_VIRT_M7_TRANSPORT_IDX_COUNT,
};

static const struct fwk_element transport_element_table[] = {
    [QEMU_VIRT_M7_TRANSPORT_IDX_SCMI] = {
        .name = "SCMI",
        .data = &((struct mod_transport_channel_config) {
            .transport_type = MOD_TRANSPORT_CHANNEL_TRANSPORT_TYPE_OUT_BAND,
            .channel_type = MOD_TRANSPORT_CHANNEL_TYPE_COMPLETER,
            .out_band_mailbox_address = UINT32_C(0x45010000),
            .out_band_mailbox_size = UINT32_C(0x10000),
            .policies = MOD_TRANSPORT_POLICY_INIT_MAILBOX,
            .driver_id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_QEMU_BRIDGE, 0),
            .driver_api_id = FWK_ID_API_INIT(FWK_MODULE_IDX_QEMU_BRIDGE, 0),
        }),
    },
    [QEMU_VIRT_M7_TRANSPORT_IDX_COUNT] = { 0 },
};

static const struct fwk_element *transport_get_element_table(fwk_id_t module_id)
{
    (void)module_id;

    return transport_element_table;
}

const struct fwk_module_config config_transport = {
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(transport_get_element_table),
};
