/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MOD_QEMU_BRIDGE_H
#define MOD_QEMU_BRIDGE_H

#include <fwk_id.h>

#include <stdint.h>

struct mod_qemu_bridge_config {
    uintptr_t bridge_reg_base;
    unsigned int irq;
};

#endif /* MOD_QEMU_BRIDGE_H */

