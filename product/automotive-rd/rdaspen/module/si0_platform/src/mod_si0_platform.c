/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     SCP platform sub-system initialization support.
 */

#include "internal/si0_platform.h"

#include <mod_si0_platform.h>

#include <fwk_id.h>
#include <fwk_log.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_status.h>

/* Module context */
struct si0_platform_ctx {
    /* Module config data */
    const struct mod_si0_platform_config *config;
};

/* Module context data */
static struct si0_platform_ctx si0_platform_ctx;

/*
 * Framework handlers
 */
static int si0_platform_mod_init(
    fwk_id_t module_id,
    unsigned int unused,
    const void *data)
{
    const struct mod_si0_platform_config *config;

    config = (struct mod_si0_platform_config *)data;

    if (!fwk_id_type_is_valid(config->transport_id)) {
        return FWK_E_DATA;
    }

    /* Save the config data in the module context */
    si0_platform_ctx.config = config;

    return FWK_SUCCESS;
}

static int si0_platform_bind(fwk_id_t id, unsigned int round)
{
    int status;

    if (round > 0) {
        return FWK_SUCCESS;
    }

    /* Bind to modules required to handshake with RSE */
    status = platform_rse_bind(si0_platform_ctx.config);
    if (status != FWK_SUCCESS) {
        return status;
    }

    return status;
}

static int si0_platform_process_bind_request(
    fwk_id_t requester_id,
    fwk_id_t target_id,
    fwk_id_t api_id,
    const void **api)
{
    int status;
    enum mod_si0_platform_api_idx api_id_type;

    api_id_type = (enum mod_si0_platform_api_idx)fwk_id_get_api_idx(api_id);

    switch (api_id_type) {
    case MOD_SI0_PLATFORM_API_IDX_TRANSPORT_SIGNAL:
        *api = get_platform_transport_signal_api();
        status = FWK_SUCCESS;
        break;

    default:
        status = FWK_E_PARAM;
    }

    return status;
}

static int si0_platform_start(fwk_id_t id)
{
    int status;

    FWK_LOG_INFO(MOD_NAME "Performing SCP-RSE handshake");

    /* Notify RSE that SYSTOP is powered up and wait for RSE doorbell */
    status = notify_rse_and_wait_for_response();
    if (status != FWK_SUCCESS) {
        FWK_LOG_ERR(MOD_NAME "Error! SCP-RSE handshake failed");
        return FWK_E_PANIC;
    }

    return status;
}

const struct fwk_module module_si0_platform = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .api_count = MOD_SI0_PLATFORM_API_COUNT,
    .init = si0_platform_mod_init,
    .bind = si0_platform_bind,
    .process_bind_request = si0_platform_process_bind_request,
    .start = si0_platform_start,
};
