/*
 * Arm SCP/MCP Software
 * Copyright (c) 2015-2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <fwk_arch.h>
#include <fwk_noreturn.h>
#include <fwk_status.h>

#include <arch_interrupt.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * Catches early failures in the initialization.
 */
static noreturn void panic(void)
{
    printf("Panic!\n");
    exit(1);
}

int main(void)
{
    int status;

    status = fwk_arch_init();
    if (status != FWK_SUCCESS)
        panic();
}
