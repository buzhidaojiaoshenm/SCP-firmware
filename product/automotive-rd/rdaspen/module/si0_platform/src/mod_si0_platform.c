/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     SCP platform sub-system initialization support.
 */

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

    /* Save the config data in the module context */
    si0_platform_ctx.config = config;

    return FWK_SUCCESS;
}

static int si0_platform_bind(fwk_id_t id, unsigned int round)
{
    return FWK_SUCCESS;
}

static int si0_platform_start(fwk_id_t id)
{
    return FWK_SUCCESS;
}

const struct fwk_module module_si0_platform = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .api_count = MOD_SI0_PLATFORM_API_COUNT,
    .init = si0_platform_mod_init,
    .bind = si0_platform_bind,
    .start = si0_platform_start,
};
