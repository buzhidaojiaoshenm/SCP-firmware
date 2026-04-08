/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

uint32_t SystemCoreClock = 25000000U;

#define QEMU_MPS2_UART0_BASE UINT32_C(0x40004000)
#define CMSDK_UART_DATA UINT32_C(0x000)
#define CMSDK_UART_STATE UINT32_C(0x004)
#define CMSDK_UART_CTRL UINT32_C(0x008)
#define CMSDK_UART_BAUDDIV UINT32_C(0x010)

#define CMSDK_UART_STATE_TXFULL UINT32_C(1 << 0)
#define CMSDK_UART_CTRL_TX_EN UINT32_C(1 << 0)

static void mmio_write32(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static uint32_t mmio_read32(uintptr_t addr)
{
    return *(volatile uint32_t *)addr;
}

static void cmsdk_uart_putc(char c)
{
    while ((mmio_read32(QEMU_MPS2_UART0_BASE + CMSDK_UART_STATE) &
            CMSDK_UART_STATE_TXFULL) != 0) {
    }

    mmio_write32(QEMU_MPS2_UART0_BASE + CMSDK_UART_DATA, (uint32_t)c);
}

static void cmsdk_uart_puts(const char *str)
{
    while (*str != '\0') {
        cmsdk_uart_putc(*str++);
    }
}

int platform_init_hook(void *params)
{
    (void)params;

    mmio_write32(
        QEMU_MPS2_UART0_BASE + CMSDK_UART_BAUDDIV,
        SystemCoreClock / UINT32_C(115200));
    mmio_write32(
        QEMU_MPS2_UART0_BASE + CMSDK_UART_CTRL,
        CMSDK_UART_CTRL_TX_EN);

    cmsdk_uart_puts("qemu_virt_m7: booting SCP firmware\r\n");

    return 0;
}
