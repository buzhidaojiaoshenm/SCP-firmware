/*
 * Arm SCP/MCP Software
 * Copyright (c) 2024-2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <fwk_arch.h>
#include <fwk_assert.h>
#include <fwk_macros.h>
#include <fwk_status.h>

#include <arch_helpers.h>

int arm_main(void)
{
    return fwk_arch_init();
}
