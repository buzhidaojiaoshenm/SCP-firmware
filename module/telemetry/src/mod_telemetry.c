/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Telemetry domain HAL (Hardware Abstraction Layer).
 *
 *     This module provides telemetry functionalities, including memory
 *     management for SHMTI (Shared Memory Telemetry Interface), bitmaps for
 *     allocation tracking, and functions for telemetry source and event
 *     management.
 */

#include <internal/telemetry.h>

#include <fwk_log.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_string.h>

#define MOD_NAME "[telemetry]"

/* Global telemetry context instance */
static struct mod_telemetry_context telemetry_ctx;

static int telemetry_shmti_init(
    const struct mod_telemetry_shmti_info *shmti_info,
    uint32_t shmti_count)
{
    int status;
    struct telemetry_shmti_context *shmti_ctx_list;
    unsigned int index;

    /* Allocate memory for SHMTI context structures */
    shmti_ctx_list =
        fwk_mm_alloc(shmti_count, sizeof(struct telemetry_shmti_context));
    if (shmti_ctx_list == NULL) {
        return FWK_E_NOMEM;
    }

    /* Initialize each SHMTI context */
    for (index = 0; index < shmti_count; index++) {
        shmti_ctx_list[index].shmti_info = &shmti_info[index];
        status = shmti_create(&shmti_ctx_list[index]);
        if (status != FWK_SUCCESS) {
            return FWK_E_PANIC;
        }
    }

    telemetry_ctx.shmti_count = shmti_count;
    telemetry_ctx.shmti_ctx_table = shmti_ctx_list;

    return FWK_SUCCESS;
}

static int telemetry_get_num_de(uint32_t *num_de)
{
    if (num_de == NULL) {
        return FWK_E_PARAM;
    }
    *num_de = telemetry_ctx.total_de_count;

    return FWK_SUCCESS;
}

/* Data Event and Event groups APIs. */
static int telemetry_get_de_desc(
    uint32_t de_index,
    struct mod_telemetry_de_desc *de_desc)
{
    return FWK_SUCCESS;
}

static int telemetry_get_de_fch_attr(
    uint32_t de_index,
    struct mod_telemetry_de_fch_attr *de_fch_attr)
{
    return FWK_SUCCESS;
}

static int telemetry_get_de_name(uint32_t de_index, char *name)
{
    return FWK_SUCCESS;
}

static int telemetry_get_enabled_de_status(
    telemetry_de_handle_st de_handle,
    struct mod_telemetry_de_status *de_status)
{
    return FWK_SUCCESS;
}

static int telemetry_get_update_intervals_info(
    uint32_t *num_intervals,
    enum mod_telemetry_update_interval_formats *interval_format)
{
    return FWK_SUCCESS;
}

static int telemetry_get_update_interval(
    uint32_t interval_index,
    uint32_t *interval)
{
    return FWK_SUCCESS;
}

static int set_sampling_rate(uint32_t sampling_rate_msecs)
{
    telemetry_ctx.current_sampling_rate_msecs = sampling_rate_msecs;
    return FWK_SUCCESS;
}

static int telemetry_enable_de_non_ts(
    uint32_t de_id,
    uint32_t shmti_id,
    telemetry_de_handle_st *de_handle,
    uint32_t *shmti_de_offset)
{
    return FWK_SUCCESS;
}

static int telemetry_enable_de_ts(
    uint32_t de_id,
    uint32_t shmti_id,
    telemetry_de_handle_st *de_handle,
    uint32_t *shmti_de_offset)
{
    return FWK_SUCCESS;
}

static int telemetry_disable_de(telemetry_de_handle_st de_handle)
{
    return FWK_SUCCESS;
}

static int telemetry_disable()
{
    /* Mark telemetry as disabled; no driver calls */
    telemetry_ctx.telemetry_enabled = false;
    return FWK_SUCCESS;
}

static int telemetry_enable()
{
    /* Mark telemetry as enabled; do not start any periodic activity */
    telemetry_ctx.telemetry_enabled = true;
    return FWK_SUCCESS;
}

static int telemetry_reset(void)
{
    return FWK_SUCCESS;
}

static struct mod_telemetry_protocol_support_api scmi_protocol_support_api = {
    /* SHMTI APIs. */
    .shmti_init = telemetry_shmti_init,
    /* Data Event fetcher APIs. */
    .get_num_de = telemetry_get_num_de,
    .get_de_desc = telemetry_get_de_desc,
    .get_de_fch_desc = telemetry_get_de_fch_attr,
    .get_de_name = telemetry_get_de_name,
    .get_enabled_de_status = telemetry_get_enabled_de_status,
    /* Update intervals APIs. */
    .get_update_intervals_info = telemetry_get_update_intervals_info,
    .get_update_interval = telemetry_get_update_interval,
    .set_sampling_rate = set_sampling_rate,
    /* Data Event configure APIs. */
    .disable_de = telemetry_disable_de,
    .enable_de_non_ts = telemetry_enable_de_non_ts,
    .enable_de_ts = telemetry_enable_de_ts,
    /* Telemetry configure APIs. */
    .telemetry_disable = telemetry_disable,
    .telemetry_enable = telemetry_enable,
    .telemetry_reset = telemetry_reset
};

static int telemetry_init(
    fwk_id_t module_id,
    unsigned int source_count,
    const void *data)
{
    const struct mod_telemetry_config *config =
        (const struct mod_telemetry_config *)data;
    if (config == NULL) {
        return FWK_E_PARAM;
    }

    telemetry_ctx.source_ctx_table =
        fwk_mm_calloc(source_count, (sizeof(struct telemetry_source_context)));
    if (telemetry_ctx.source_ctx_table == NULL) {
        return FWK_E_NOMEM;
    }
    /*! Initialise telemetry context. */
    telemetry_ctx.config = config;
    telemetry_ctx.num_sources = source_count;
    telemetry_ctx.total_de_count = 0;
    telemetry_ctx.total_de_enabled_count = 0;
    telemetry_ctx.telemetry_enabled = false;

    return FWK_SUCCESS;
}

static int telemetry_element_init(
    fwk_id_t source_id,
    unsigned int unused,
    const void *data)
{
    unsigned int source_idx;
    const struct mod_telemetry_source_config *config =
        (const struct mod_telemetry_source_config *)data;
    if (!fwk_module_is_valid_element_id(source_id) || config == NULL) {
        return FWK_E_PARAM;
    }

    source_idx = fwk_id_get_element_idx(source_id);
    telemetry_ctx.source_ctx_table[source_idx].config = config;

    return FWK_SUCCESS;
}

static int telemetry_bind(fwk_id_t id, unsigned int round)
{
    struct telemetry_source_context *source_ctx;

    if (round == 0 || fwk_id_is_type(id, FWK_ID_TYPE_MODULE)) {
        return FWK_SUCCESS;
    }
    if (!fwk_module_is_valid_element_id(id) ||
        fwk_id_get_element_idx(id) >= telemetry_ctx.num_sources) {
        return FWK_E_PARAM;
    }

    source_ctx = &telemetry_ctx.source_ctx_table[fwk_id_get_element_idx(id)];
    return fwk_module_bind(
        FWK_ID_MODULE(fwk_id_get_module_idx(source_ctx->config->drv_api_id)),
        source_ctx->config->drv_api_id,
        &source_ctx->api);
}

static int telemetry_process_bind_request(
    fwk_id_t source_id,
    fwk_id_t target_id,
    fwk_id_t api_id,
    const void **api)
{
    if (api == NULL) {
        return FWK_E_PARAM;
    }

    enum mod_telemetry_api_idx api_idx =
        (enum mod_telemetry_api_idx)fwk_id_get_api_idx(api_id);
    if (api_idx != MOD_TELEMETRY_API_IDX_PROTOCOL_SUPPORT) {
        return FWK_E_PARAM;
    }
    *api = &scmi_protocol_support_api;

    return FWK_SUCCESS;
}

static int telemetry_start(fwk_id_t id)
{
    int status;
    struct telemetry_source_context *source_ctx;

    if (fwk_id_is_type(id, FWK_ID_TYPE_MODULE)) {
        return FWK_SUCCESS;
    }

    if (!fwk_module_is_valid_element_id(id) ||
        fwk_id_get_element_idx(id) >= telemetry_ctx.num_sources) {
        return FWK_E_PARAM;
    }

    source_ctx = &telemetry_ctx.source_ctx_table[fwk_id_get_element_idx(id)];
    status =
        source_ctx->api->get_de_list(&source_ctx->num_de, &source_ctx->de_list);
    if (status != FWK_SUCCESS) {
        return status;
    }
    /* Update the total DE count */
    telemetry_ctx.total_de_count += source_ctx->num_de;

    return FWK_SUCCESS;
}

const struct fwk_module module_telemetry = {
    .type = FWK_MODULE_TYPE_HAL,
    .api_count = (unsigned int)MOD_TELEMETRY_API_IDX_COUNT,
    .init = telemetry_init,
    .element_init = telemetry_element_init,
    .bind = telemetry_bind,
    .process_bind_request = telemetry_process_bind_request,
    .start = telemetry_start,
};
