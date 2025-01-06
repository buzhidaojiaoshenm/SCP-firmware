/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Configuration data for module 'si0_platform'.
 */

#include <mod_si0_platform.h>

#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

enum scp_cfgd_mod_transport_element_idx {
    SI0_CFGD_MOD_TRANSPORT_EIDX_SYSTEM,
    SI0_CFGD_MOD_TRANSPORT_EIDX_COUNT,
};

static const struct mod_si0_platform_config platform_config_data = {
    .transport_id = FWK_ID_ELEMENT_INIT(
        FWK_MODULE_IDX_TRANSPORT,
        SI0_CFGD_MOD_TRANSPORT_EIDX_SYSTEM),
};

struct fwk_module_config config_si0_platform = {
    .data = &platform_config_data,
};
