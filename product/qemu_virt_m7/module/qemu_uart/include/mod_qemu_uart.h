/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MOD_QEMU_UART_H
#define MOD_QEMU_UART_H

#include <stdint.h>

struct mod_qemu_uart_config {
    uintptr_t base;
    uint32_t clock_hz;
    uint32_t baudrate;
};

#endif /* MOD_QEMU_UART_H */
