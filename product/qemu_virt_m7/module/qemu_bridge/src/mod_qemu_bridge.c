/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_qemu_bridge.h>
#include <mod_transport.h>

#include <fwk_id.h>
#include <fwk_interrupt.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_status.h>

#include <stddef.h>
#include <stdint.h>

#define BRIDGE_REG_IRQ_ACK UINT32_C(0x0c)
#define BRIDGE_REG_DOORBELL UINT32_C(0x08)

struct qemu_bridge_ctx {
    const struct mod_qemu_bridge_config *config;
    const struct mod_transport_driver_input_api *transport_driver_input_api;
    fwk_id_t transport_id;
};

static struct qemu_bridge_ctx *ctx_table;

static void mmio_write32(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static void qemu_bridge_isr(uintptr_t param)
{
    struct qemu_bridge_ctx *ctx = (struct qemu_bridge_ctx *)param;

    mmio_write32(ctx->config->bridge_reg_base + BRIDGE_REG_IRQ_ACK, 1);
    (void)ctx->transport_driver_input_api->signal_message(ctx->transport_id);
}

static int qemu_bridge_trigger_event(fwk_id_t dev_id)
{
    struct qemu_bridge_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    mmio_write32(ctx->config->bridge_reg_base + BRIDGE_REG_DOORBELL, 1);
    return FWK_SUCCESS;
}

static const struct mod_transport_driver_api qemu_bridge_driver_api = {
    .trigger_event = qemu_bridge_trigger_event,
};

static int qemu_bridge_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    (void)module_id;
    (void)data;

    ctx_table = fwk_mm_calloc(element_count, sizeof(ctx_table[0]));
    if (ctx_table == NULL) {
        return FWK_E_NOMEM;
    }

    return FWK_SUCCESS;
}

static int qemu_bridge_element_init(
    fwk_id_t element_id,
    unsigned int unused,
    const void *data)
{
    struct qemu_bridge_ctx *ctx = &ctx_table[fwk_id_get_element_idx(element_id)];

    (void)unused;

    ctx->config = data;
    if ((ctx->config == NULL) || (ctx->config->bridge_reg_base == 0) ||
        (ctx->config->irq == 0)) {
        return FWK_E_DATA;
    }

    return FWK_SUCCESS;
}

static int qemu_bridge_bind(fwk_id_t id, unsigned int round)
{
    struct qemu_bridge_ctx *ctx;

    if ((round != 0) || !fwk_module_is_valid_element_id(id)) {
        return FWK_SUCCESS;
    }

    ctx = &ctx_table[fwk_id_get_element_idx(id)];
    ctx->transport_id =
        FWK_ID_ELEMENT(FWK_MODULE_IDX_TRANSPORT, fwk_id_get_element_idx(id));

    return fwk_module_bind(
        ctx->transport_id,
        FWK_ID_API(FWK_MODULE_IDX_TRANSPORT, MOD_TRANSPORT_API_IDX_DRIVER_INPUT),
        &ctx->transport_driver_input_api);
}

static int qemu_bridge_start(fwk_id_t id)
{
    struct qemu_bridge_ctx *ctx;
    int status;

    if (!fwk_module_is_valid_element_id(id)) {
        return FWK_SUCCESS;
    }

    ctx = &ctx_table[fwk_id_get_element_idx(id)];

    status = fwk_interrupt_set_isr_param(
        ctx->config->irq, qemu_bridge_isr, (uintptr_t)ctx);
    if (status != FWK_SUCCESS) {
        return status;
    }

    status = fwk_interrupt_clear_pending(ctx->config->irq);
    if (status != FWK_SUCCESS) {
        return status;
    }

    return fwk_interrupt_enable(ctx->config->irq);
}

static int qemu_bridge_process_bind_request(
    fwk_id_t requester_id,
    fwk_id_t id,
    fwk_id_t api_id,
    const void **api)
{
    (void)requester_id;
    (void)id;

    if (!fwk_id_is_equal(api_id, FWK_ID_API(FWK_MODULE_IDX_QEMU_BRIDGE, 0))) {
        return FWK_E_PARAM;
    }

    *api = &qemu_bridge_driver_api;
    return FWK_SUCCESS;
}

static const struct fwk_element qemu_bridge_element_table[] = {
    [0] = {
        .name = "SCMI",
        .data = &(struct mod_qemu_bridge_config) {
            .bridge_reg_base = UINT32_C(0x40014000),
            .irq = 11,
        },
    },
    [1] = { 0 },
};

const struct fwk_module_config config_qemu_bridge = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(qemu_bridge_element_table),
};

const struct fwk_module module_qemu_bridge = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .api_count = 1,
    .init = qemu_bridge_init,
    .element_init = qemu_bridge_element_init,
    .bind = qemu_bridge_bind,
    .start = qemu_bridge_start,
    .process_bind_request = qemu_bridge_process_bind_request,
};
