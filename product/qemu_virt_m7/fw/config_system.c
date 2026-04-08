/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

uint32_t SystemCoreClock = 25000000U;

int platform_init_hook(void *params)
{
    (void)params;

    return 0;
}
