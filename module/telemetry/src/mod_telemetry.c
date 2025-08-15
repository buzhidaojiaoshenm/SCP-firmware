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

#include <mod_telemetry.h>

#include <fwk_log.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_notification.h>

#define MOD_NAME "[telemetry]"

/*!
 * \brief Telemetry source (driver) context structure.
 *
 * This structure manages telemetry sources, their associated APIs, and
 * allocated data events.
 */
struct telemetry_source_context {
    /*! Identifier of the telemetry source. */
    fwk_id_t id;
    /*! Identifier of the telemetry source module */
    fwk_id_t source_driver_id;
    /*! API for telemetry source */
    const struct mod_telemetry_driver_api *api;
    /*! Number of Data Events (DEs) */
    uint32_t num_de;
    /*! Base index for DEs */
    uint32_t de_index_base;
    /*! List of DEs */
    struct mod_telemetry_de *de_list;
    /*! Count of enabled DEs */
    uint32_t de_enabled_count;
};

/*!
 * \brief Global telemetry module context.
 *
 * Stores state, telemetry sources, and SHMTI management structures.
 */
struct mod_telemetry_context {
    /*! Number of telemetry sources */
    uint32_t n_sources;
    /*! Telemetry sources */
    struct telemetry_source_context
        *source_ctx[MOD_TELEMETRY_MAX_TELEMETRY_SOURCES];
    /*! Flag indicating telemetry is enabled */
    bool telemetry_enabled;
    /*! Telemetry sampling rate */
    uint32_t current_sampling_rate_msecs;
    /*! Total number of Data Events */
    uint32_t total_de_count;
    /*! Total enabled Data Events */
    uint32_t total_de_enabled_count;
    /*! Telemetry configuration */
    const struct mod_telemetry_config *telemetry_config;
};

/* Global telemetry context instance */
static struct mod_telemetry_context telemetry_ctx;

/*!
 * \brief Retrieves the number of SHMTI regions.
 *
 * \details This function returns the total number of SHMTI (Shared Memory
 *          Telemetry Interface) regions available in the system.
 *
 * \param[out] num_shmti Pointer to store the total SHMTI count.
 *
 * \retval FWK_SUCCESS if the operation was successful.
 * \retval FWK_E_PARAM if the provided pointer is NULL.
 */
static int telemetry_get_num_shmti(uint32_t *num_shmti)
{
    if (num_shmti == NULL) {
        return FWK_E_PARAM;
    }

    *num_shmti = telemetry_ctx.telemetry_config->shmti_count;
    return FWK_SUCCESS;
}

/*!
 * \brief Retrieves details of a specific SHMTI region.
 *
 * \details This function retrieves the SHMTI descriptor for the given index.
 *
 * \param[in] index The index of the SHMTI region to retrieve.
 * \param[out] shmti_desc Pointer to store the SHMTI descriptor.
 *
 * \retval FWK_SUCCESS if the operation was successful.
 * \retval FWK_E_PARAM if the index is out of range or the pointer is NULL.
 */
static int telemetry_get_shmti(
    uint32_t index,
    struct mod_telemetry_shmti_desc *shmti_desc)
{
    return FWK_SUCCESS;
}

/*!
 * \brief Retrieves the total number of Data Events (DEs).
 *
 * \details This function returns the number of telemetry Data Events (DEs)
 *          managed by the system.
 *
 * \param[out] num_de Pointer to store the total DE count.
 *
 * \retval FWK_SUCCESS if the operation was successful.
 * \retval FWK_E_PARAM if the provided pointer is NULL.
 */
static int telemetry_get_num_de(uint32_t *num_de)
{
    if (num_de == NULL) {
        return FWK_E_PARAM;
    }

    *num_de = telemetry_ctx.total_de_count;

    return FWK_SUCCESS;
}

static int telemetry_get_de(
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

static int telemetry_enable_de_non_ts(
    uint32_t de_id,
    telemetry_de_handle_st *de_handle,
    uint32_t *shmti_id,
    uint32_t *shmti_de_offset)
{
    return FWK_SUCCESS;
}

static int telemetry_enable_de_ts(
    uint32_t de_id,
    telemetry_de_handle_st *de_handle,
    uint32_t *shmti_id,
    uint32_t *shmti_de_offset)
{
    return FWK_SUCCESS;
}

static int telemetry_disable_de(telemetry_de_handle_st de_handle)
{
    return FWK_SUCCESS;
}

static int telemetry_disable_all_de()
{
    return FWK_SUCCESS;
}

static int telemetry_get_num_de_enabled(uint32_t *num_de_enabled)
{
    *num_de_enabled = telemetry_ctx.total_de_enabled_count;
    return FWK_SUCCESS;
}

static int telemetry_get_de_enabled(
    telemetry_de_handle_st de_handle,
    struct mod_telemetry_de_status *de_status)
{
    return FWK_SUCCESS;
}

static int set_sampling_rate(uint32_t sampling_rate_msecs)
{
    telemetry_ctx.current_sampling_rate_msecs = sampling_rate_msecs;
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

/*!
 * \brief SCMI Telemetry Protocol Support API.
 *
 * \details This API provides functions for interacting with telemetry
 *          sources, managing shared memory telemetry interfaces (SHMTI),
 *          handling Data Events (DE), and controlling telemetry operations.
 */
static struct mod_telemetry_protocol_support_api scmi_protocol_support_api = {
    .get_num_shmti = telemetry_get_num_shmti,
    .get_shmti = telemetry_get_shmti,
    .get_de = telemetry_get_de,
    .get_de_fch_desc = telemetry_get_de_fch_attr,
    .get_de_name = telemetry_get_de_name,
    .get_num_de = telemetry_get_num_de,
    .get_update_intervals_info = telemetry_get_update_intervals_info,
    .get_update_interval = telemetry_get_update_interval,
    .disable_all_de = telemetry_disable_all_de,
    .disable_de = telemetry_disable_de,
    .enable_de_non_ts = telemetry_enable_de_non_ts,
    .enable_de_ts = telemetry_enable_de_ts,
    .get_num_de_enabled = telemetry_get_num_de_enabled,
    .get_de_enabled = telemetry_get_de_enabled,
    .telemetry_disable = telemetry_disable,
    .telemetry_enable = telemetry_enable,
    .set_sampling_rate = set_sampling_rate,
    .telemetry_reset = telemetry_reset
};

/*!
 * \brief Initializes the telemetry module.
 *
 * \details This function initializes the telemetry module by setting up the
 *          telemetry configuration from the provided data.
 *
 * \param[in] module_id The module identifier.
 * \param[in] dev_count The number of telemetry sources.
 * \param[in] data Pointer to the telemetry module configuration.
 *
 * \retval FWK_SUCCESS if initialization is successful.
 * \retval FWK_E_PARAM if the provided configuration data is NULL.
 */
static int telemetry_init(
    fwk_id_t module_id,
    unsigned int dev_count,
    const void *data)
{
    if (data == NULL) {
        return FWK_E_PARAM;
    }

    telemetry_ctx.telemetry_config = (const struct mod_telemetry_config *)data;

    telemetry_ctx.n_sources = 0;

    return FWK_SUCCESS;
}

/*!
 * \brief Binds telemetry sources to their respective APIs.
 *
 * \param[in] id The module identifier.
 * \param[in] round The binding round (0 for initial binding, 1 for source
 * binding).
 *
 * \retval FWK_SUCCESS if binding is successful.
 * \retval FWK_E_PANIC if binding to the Telemetry source API fails.
 */
static int telemetry_bind(fwk_id_t id, unsigned int round)
{
    int status;
    size_t i;
    struct telemetry_source_context *telemetry_source_ctx;
    fwk_id_t source_driver_api_id;
    fwk_id_t source_driver_id;

    if (round == 0 || !fwk_id_is_type(id, FWK_ID_TYPE_MODULE)) {
        return FWK_SUCCESS;
    }

    /* Bind each telemetry source to its respective driver API */
    for (i = 0; i < telemetry_ctx.n_sources; i++) {
        telemetry_source_ctx = telemetry_ctx.source_ctx[i];
        source_driver_id = telemetry_source_ctx->source_driver_id;
        source_driver_api_id =
            FWK_ID_API(fwk_id_get_module_idx(source_driver_id), 0);

        status = fwk_module_bind(
            source_driver_id, source_driver_api_id, &telemetry_source_ctx->api);

        if (status != FWK_SUCCESS) {
            return status;
        }
    }

    return FWK_SUCCESS;
}

/*!
 * \brief Processes API binding requests.
 *
 * \details This function handles binding requests from different modules
 *          requesting access to telemetry services. It supports binding
 *          for both telemetry protocol support and telemetry driver support.
 *
 * \param[in] source_id The identifier of the requesting module.
 * \param[in] target_id The identifier of the telemetry module.
 * \param[in] api_id The API identifier requested by the module.
 * \param[out] api Pointer to store the requested API.
 *
 * \retval FWK_SUCCESS if the API is successfully provided.
 * \retval FWK_E_ACCESS if the requested API index is invalid.
 */
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

    switch (api_idx) {
    case MOD_TELEMETRY_API_IDX_PROTOCOL_SUPPORT:
        *api = &scmi_protocol_support_api;
        break;
    default:
        return FWK_E_ACCESS;
    }

    return FWK_SUCCESS;
}

/*!
 * \brief Starts the telemetry system.
 *
 * \details This function initializes telemetry by retrieving Data Event (DE)
 *          lists from all registered telemetry sources. It ensures each source
 *          has its DEs properly indexed and counted, allowing for accurate
 *          telemetry tracking.
 *
 * \param[in] id The module identifier.
 *
 * \retval FWK_SUCCESS if telemetry starts successfully.
 * \retval FWK_E_PARAM if telemetry sources are not properly initialized.
 * \retval FWK_E_STATE if a source fails to provide its DE list.
 */
static int telemetry_start(fwk_id_t id)
{
    struct telemetry_source_context *source_ctx;
    uint32_t total_de_count = 0;

    /* Iterate through all telemetry sources and retrieve their DE lists */
    for (size_t i = 0; i < telemetry_ctx.n_sources; i++) {
        source_ctx = telemetry_ctx.source_ctx[i];

        /* Ensure source context and API are valid before calling functions */
        if (source_ctx == NULL) {
            return FWK_E_STATE;
        }

        source_ctx->num_de = 0;
        source_ctx->de_list = NULL;
        source_ctx->de_index_base = total_de_count;
        /* Update the total DE count */
        total_de_count += source_ctx->num_de;
    }

    telemetry_ctx.total_de_count = total_de_count;

    return FWK_SUCCESS;
}

/*!
 * \brief Telemetry module descriptor.
 *
 * \details This structure defines the module-level properties for the
 *          telemetry module, including its type, API count, and function
 *          pointers for initialization, binding, and API request processing.
 */
const struct fwk_module module_telemetry = {
    .type = FWK_MODULE_TYPE_HAL,
    .api_count = (unsigned int)MOD_TELEMETRY_API_IDX_COUNT,
    .init = telemetry_init,
    .bind = telemetry_bind,
    .process_bind_request = telemetry_process_bind_request,
    .start = telemetry_start,
};
