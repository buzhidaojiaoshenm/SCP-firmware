/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MOD_QEMU_TIMER_H
#define MOD_QEMU_TIMER_H

#include <stdint.h>

struct mod_qemu_timer_dev_config {
    uintptr_t alarm_timer_base;
    uintptr_t counter_timer_base;
    uint32_t frequency;
};

#endif /* MOD_QEMU_TIMER_H */
