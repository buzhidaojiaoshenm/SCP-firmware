/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     SCP Platform Support - implements support for communication with RSE.
 */

#include <internal/si0_platform.h>

#include <mod_si0_platform.h>
#include <mod_transport.h>

#include <fwk_arch.h>
#include <fwk_assert.h>
#include <fwk_id.h>
#include <fwk_log.h>
#include <fwk_module.h>
#include <fwk_status.h>

#include <stdbool.h>

/* Platform RSE context */
struct platform_rse_ctx {
    /* Pointer to the module config data */
    const struct mod_si0_platform_config *config;

    /* Transport API to send/respond to a message */
    const struct mod_transport_firmware_api *transport_api;

    /* Flag to indicate that the RSE doorbell has been received */
    volatile bool rse_doorbell_received;
};

static struct platform_rse_ctx ctx;

/* Utility function to check if SI0 platform has received doorbell from RSE */
static bool is_rse_doorbell_received(void *unused)
{
    return ctx.rse_doorbell_received;
}

/*
 * Module 'transport' signal interface implementation.
 */
static int signal_error(fwk_id_t unused)
{
    fwk_assert(ctx.transport_api != NULL);

    FWK_LOG_ERR(MOD_NAME "Error! Invalid response received from RSE");

    ctx.transport_api->release_transport_channel_lock(ctx.config->transport_id);

    return FWK_SUCCESS;
}

static int signal_message(fwk_id_t unused)
{
    fwk_assert(ctx.transport_api != NULL);

    FWK_LOG_INFO(MOD_NAME "Received doorbell event from RSE");

    ctx.transport_api->release_transport_channel_lock(ctx.config->transport_id);

    /* Set the flag to indicate that the RSE initialization is complete */
    ctx.rse_doorbell_received = true;

    return FWK_SUCCESS;
}

const struct mod_transport_firmware_signal_api platform_transport_signal_api = {
    .signal_error = signal_error,
    .signal_message = signal_message,
};

/*
 * Helper function to retrieve the 'transport' module signal API.
 */
const void *get_platform_transport_signal_api(void)
{
    return &platform_transport_signal_api;
}

/*
 * RSE has to be notified that SYSTOP is powered up and the CMN has been
 * configured.
 */
int notify_rse_and_wait_for_response(void)
{
    int status;

    fwk_assert(ctx.transport_api != NULL);

    /*
     * Trigger doorbell to RSE to indicate that the SYSTOP domain is ON.
     */
    status = ctx.transport_api->trigger_interrupt(ctx.config->transport_id);
    if (status != FWK_SUCCESS) {
        FWK_LOG_ERR(MOD_NAME
                    "Error! Failed to send SYSTOP enabled message to RSE");
        return status;
    }

    /*
     * Wait till a doorbell from RSE is received. This doorbell event indicates
     * that the RSE has initialized and completed the peripheral interconnect
     * setup.
     */
    while (!is_rse_doorbell_received(NULL)) {
        fwk_arch_suspend();
    }

    return FWK_SUCCESS;
}

/*
 * Bind to timer and transport module to communicate with RSE.
 */
int platform_rse_bind(const struct mod_si0_platform_config *config)
{
    fwk_id_t transport_api_id;

    ctx.config = config;

    transport_api_id =
        FWK_ID_API(FWK_MODULE_IDX_TRANSPORT, MOD_TRANSPORT_API_IDX_FIRMWARE);
    return fwk_module_bind(
        config->transport_id, transport_api_id, &ctx.transport_api);
}
