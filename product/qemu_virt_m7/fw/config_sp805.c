/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_sp805.h>

#include <fwk_interrupt.h>
#include <fwk_module.h>

#define QEMU_VIRT_M7_SP805_BASE UINT32_C(0x44006000)
#define QEMU_VIRT_M7_SP805_LOAD UINT32_C(0x0fffffff)

const struct fwk_module_config config_sp805 = {
    .data = &((struct mod_sp805_config) {
        .reg_base = QEMU_VIRT_M7_SP805_BASE,
        .wdt_load_value = QEMU_VIRT_M7_SP805_LOAD,
        .driver_id = FWK_ID_NONE_INIT,
        .sp805_irq = FWK_INTERRUPT_NMI,
    }),
};
