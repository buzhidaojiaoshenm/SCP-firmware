/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Configuration data for module 'transport'.
 */

#include "si0_cfgd_mhu3.h"
#include "si0_cfgd_transport.h"
#include "si0_mmap.h"

#include <mod_mhu3.h>
#include <mod_si0_platform.h>
#include <mod_transport.h>

#include <fwk_element.h>
#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

/* Module 'transport' element configuration table */
static const struct fwk_element element_table[]  = {
    [SI0_CFGD_MOD_TRANSPORT_EIDX_SYSTEM] = {
        .name = "SI0_PLATFORM_TRANSPORT",
        .data = &((
            struct mod_transport_channel_config) {
                .transport_type = MOD_TRANSPORT_CHANNEL_TRANSPORT_TYPE_NONE,
                .policies = MOD_TRANSPORT_POLICY_NONE,
                .channel_type = MOD_TRANSPORT_CHANNEL_TYPE_COMPLETER,
                .signal_api_id =
                    FWK_ID_API_INIT(
                        FWK_MODULE_IDX_SI0_PLATFORM,
                        MOD_SI0_PLATFORM_API_IDX_TRANSPORT_SIGNAL),
                .driver_id =
                    FWK_ID_SUB_ELEMENT_INIT(
                        FWK_MODULE_IDX_MHU3,
                        SI0_CFGD_MOD_MHU3_EIDX_SI0_RSE,
                        0),
                .driver_api_id =
                    FWK_ID_API_INIT(
                        FWK_MODULE_IDX_MHU3,
                        MOD_MHU3_API_IDX_TRANSPORT_DRIVER),
        }),
    },
    [SI0_CFGD_MOD_TRANSPORT_EIDX_COUNT] = { 0 },
};

const struct fwk_module_config config_transport = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(element_table),
};
