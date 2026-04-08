/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_qemu_timer.h>
#include <mod_timer.h>

#include <fwk_id.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_status.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CMSDK_TIMER_CTRL UINT32_C(0x000)
#define CMSDK_TIMER_VALUE UINT32_C(0x004)
#define CMSDK_TIMER_RELOAD UINT32_C(0x008)
#define CMSDK_TIMER_INTSTATUS UINT32_C(0x00C)

#define CMSDK_TIMER_CTRL_EN UINT32_C(1 << 0)
#define CMSDK_TIMER_CTRL_IRQEN UINT32_C(1 << 3)
#define CMSDK_TIMER_MAX_RELOAD UINT32_MAX

struct qemu_timer_ctx {
    const struct mod_qemu_timer_dev_config *config;
};

static struct qemu_timer_ctx *ctx_table;

static void mmio_write32(uintptr_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
}

static uint32_t mmio_read32(uintptr_t addr)
{
    return *(volatile uint32_t *)addr;
}

static int qemu_timer_enable(fwk_id_t dev_id)
{
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    mmio_write32(
        ctx->config->alarm_timer_base + CMSDK_TIMER_CTRL,
        CMSDK_TIMER_CTRL_EN | CMSDK_TIMER_CTRL_IRQEN);

    return FWK_SUCCESS;
}

static int qemu_timer_disable(fwk_id_t dev_id)
{
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    mmio_write32(ctx->config->alarm_timer_base + CMSDK_TIMER_CTRL, 0);
    mmio_write32(ctx->config->alarm_timer_base + CMSDK_TIMER_INTSTATUS, 1);

    return FWK_SUCCESS;
}

static int qemu_timer_get_counter(fwk_id_t dev_id, uint64_t *value)
{
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    *value = (uint64_t)(
        CMSDK_TIMER_MAX_RELOAD -
        mmio_read32(ctx->config->counter_timer_base + CMSDK_TIMER_VALUE));

    return FWK_SUCCESS;
}

static int qemu_timer_set_timer(fwk_id_t dev_id, uint64_t timestamp)
{
    uint64_t counter;
    uint64_t delta;
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    qemu_timer_get_counter(dev_id, &counter);
    delta = (timestamp > counter) ? (timestamp - counter) : 1;

    if (delta > CMSDK_TIMER_MAX_RELOAD) {
        delta = CMSDK_TIMER_MAX_RELOAD;
    }

    mmio_write32(ctx->config->alarm_timer_base + CMSDK_TIMER_CTRL, 0);
    mmio_write32(ctx->config->alarm_timer_base + CMSDK_TIMER_INTSTATUS, 1);
    mmio_write32(
        ctx->config->alarm_timer_base + CMSDK_TIMER_RELOAD,
        (uint32_t)delta);
    mmio_write32(
        ctx->config->alarm_timer_base + CMSDK_TIMER_VALUE,
        (uint32_t)delta);

    return FWK_SUCCESS;
}

static int qemu_timer_get_timer(fwk_id_t dev_id, uint64_t *timestamp)
{
    uint64_t counter;
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    qemu_timer_get_counter(dev_id, &counter);
    *timestamp = counter +
        mmio_read32(ctx->config->alarm_timer_base + CMSDK_TIMER_VALUE);

    return FWK_SUCCESS;
}

static int qemu_timer_get_frequency(fwk_id_t dev_id, uint32_t *value)
{
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(dev_id)];

    *value = ctx->config->frequency;

    return FWK_SUCCESS;
}

static fwk_timestamp_t mod_qemu_timer_timestamp(const void *ctx)
{
    const struct mod_qemu_timer_dev_config *config = ctx;
    uint64_t counter;

    if (config == NULL) {
        return 0;
    }

    counter = (uint64_t)(
        CMSDK_TIMER_MAX_RELOAD -
        mmio_read32(config->counter_timer_base + CMSDK_TIMER_VALUE));

    return (FWK_S(1) / config->frequency) * counter;
}

static const struct mod_timer_driver_api qemu_timer_driver_api = {
    .name = "qemu-timer",
    .enable = qemu_timer_enable,
    .disable = qemu_timer_disable,
    .set_timer = qemu_timer_set_timer,
    .get_timer = qemu_timer_get_timer,
    .get_counter = qemu_timer_get_counter,
    .get_frequency = qemu_timer_get_frequency,
};

static int qemu_timer_init(
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

static int qemu_timer_element_init(
    fwk_id_t element_id,
    unsigned int unused,
    const void *data)
{
    struct qemu_timer_ctx *ctx = &ctx_table[fwk_id_get_element_idx(element_id)];

    (void)unused;

    ctx->config = data;
    if ((ctx->config == NULL) || (ctx->config->alarm_timer_base == 0) ||
        (ctx->config->counter_timer_base == 0) ||
        (ctx->config->frequency == 0)) {
        return FWK_E_DATA;
    }

    mmio_write32(ctx->config->counter_timer_base + CMSDK_TIMER_CTRL, 0);
    mmio_write32(
        ctx->config->counter_timer_base + CMSDK_TIMER_RELOAD,
        CMSDK_TIMER_MAX_RELOAD);
    mmio_write32(
        ctx->config->counter_timer_base + CMSDK_TIMER_VALUE,
        CMSDK_TIMER_MAX_RELOAD);
    mmio_write32(
        ctx->config->counter_timer_base + CMSDK_TIMER_CTRL,
        CMSDK_TIMER_CTRL_EN);

    return qemu_timer_disable(element_id);
}

static int qemu_timer_process_bind_request(
    fwk_id_t requester_id,
    fwk_id_t id,
    fwk_id_t api_id,
    const void **api)
{
    (void)requester_id;
    (void)id;

    if (!fwk_id_is_equal(
            api_id,
            FWK_ID_API(FWK_MODULE_IDX_QEMU_TIMER, 0))) {
        return FWK_E_PARAM;
    }

    *api = &qemu_timer_driver_api;

    return FWK_SUCCESS;
}

static const struct fwk_element qemu_timer_element_table[] = {
    [0] = {
        .name = "REFCLK",
        .data = &(struct mod_qemu_timer_dev_config) {
            .alarm_timer_base = UINT32_C(0x40000000),
            .counter_timer_base = UINT32_C(0x40001000),
            .frequency = UINT32_C(25000000),
        },
    },
    [1] = { 0 },
};

const struct fwk_module_config config_qemu_timer = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(qemu_timer_element_table),
};

struct fwk_time_driver mod_qemu_timer_driver(
    const void **ctx,
    const struct mod_qemu_timer_dev_config *cfg)
{
    *ctx = cfg;

    return (struct fwk_time_driver){
        .timestamp = mod_qemu_timer_timestamp,
    };
}

const struct fwk_module module_qemu_timer = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .api_count = 1,
    .init = qemu_timer_init,
    .element_init = qemu_timer_element_init,
    .process_bind_request = qemu_timer_process_bind_request,
};
