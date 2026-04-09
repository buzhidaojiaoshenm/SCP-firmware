/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_system_info.h>

#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

const struct fwk_module_config config_system_info = {
    .data = &((struct mod_system_info_config){
        .system_info_driver_module_id = FWK_ID_MODULE_INIT(
            FWK_MODULE_IDX_QEMU_SYSINFO),
        .system_info_driver_data_api_id = FWK_ID_API_INIT(
            FWK_MODULE_IDX_QEMU_SYSINFO,
            0),
    }),
};
