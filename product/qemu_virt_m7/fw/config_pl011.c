/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_pl011.h>

#include <fwk_element.h>
#include <fwk_id.h>
#include <fwk_macros.h>
#include <fwk_module.h>

static const struct fwk_element config_pl011_elements[] = {
    [0] = {
        .name = "scp_uart",
        .data = &((struct mod_pl011_element_cfg) {
            .reg_base = UINT32_C(0x44002000),
            .baud_rate_bps = 115200,
            .clock_rate_hz = 24 * FWK_MHZ,
        }),
    },
    [1] = { 0 },
};

const struct fwk_module_config config_pl011 = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(config_pl011_elements),
};
