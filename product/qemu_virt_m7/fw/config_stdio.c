/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <fwk_module.h>

static const struct fwk_element config_stdio_elements[] = {
    [0] = { 0 },
};

const struct fwk_module_config config_stdio = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(config_stdio_elements),
};

