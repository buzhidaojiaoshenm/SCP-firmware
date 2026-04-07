/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_qemu_uart.h>

#include <fwk_io.h>
#include <fwk_module.h>
#include <fwk_status.h>

#include <stdint.h>

#define CMSDK_UART_DATA UINT32_C(0x000)
#define CMSDK_UART_STATE UINT32_C(0x004)
#define CMSDK_UART_CTRL UINT32_C(0x008)
#define CMSDK_UART_BAUDDIV UINT32_C(0x010)

#define CMSDK_UART_STATE_TXFULL UINT32_C(1 << 0)
#define CMSDK_UART_CTRL_TX_EN UINT32_C(1 << 0)

static const struct mod_qemu_uart_config *qemu_uart_config;

static void mmio_write32(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static uint32_t mmio_read32(uintptr_t addr)
{
    return *(volatile uint32_t *)addr;
}

static void qemu_uart_write_char(char ch)
{
    while ((mmio_read32(qemu_uart_config->base + CMSDK_UART_STATE) &
            CMSDK_UART_STATE_TXFULL) != 0) {
    }

    mmio_write32(qemu_uart_config->base + CMSDK_UART_DATA, (uint32_t)ch);
}

static int qemu_uart_open(const struct fwk_io_stream *stream)
{
    if (qemu_uart_config == NULL) {
        qemu_uart_config = fwk_module_get_data(stream->id);
    }

    if ((qemu_uart_config == NULL) || (qemu_uart_config->base == 0) ||
        (qemu_uart_config->clock_hz == 0) ||
        (qemu_uart_config->baudrate == 0)) {
        return FWK_E_DATA;
    }

    mmio_write32(
        qemu_uart_config->base + CMSDK_UART_BAUDDIV,
        qemu_uart_config->clock_hz / qemu_uart_config->baudrate);
    mmio_write32(
        qemu_uart_config->base + CMSDK_UART_CTRL,
        CMSDK_UART_CTRL_TX_EN);

    return FWK_SUCCESS;
}

static int qemu_uart_putch(const struct fwk_io_stream *stream, char ch)
{
    (void)stream;

    if (ch == '\0') {
        return FWK_SUCCESS;
    }

    qemu_uart_write_char(ch);

    return FWK_SUCCESS;
}

static int qemu_uart_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    (void)module_id;
    (void)element_count;

    qemu_uart_config = data;

    return FWK_SUCCESS;
}

const struct fwk_module_config config_qemu_uart = {
    .data = &(struct mod_qemu_uart_config) {
        .base = UINT32_C(0x40004000),
        .clock_hz = UINT32_C(25000000),
        .baudrate = UINT32_C(115200),
    },
};

const struct fwk_module module_qemu_uart = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = qemu_uart_init,
    .adapter = {
        .open = qemu_uart_open,
        .putch = qemu_uart_putch,
    },
};
