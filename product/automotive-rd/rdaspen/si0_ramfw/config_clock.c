/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Configuration data for module 'clock'.
 */

#include <mod_clock.h>

#include <fwk_element.h>
#include <fwk_module.h>

static const struct fwk_element clock_dev_table[] = {
    { 0 },
};

const struct fwk_module_config config_clock = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(clock_dev_table),
};
