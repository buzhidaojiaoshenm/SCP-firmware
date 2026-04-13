/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_cmn_cyprus.h>
#include <mod_scmi.h>
#include <mod_scmi_std.h>

#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_status.h>

#include <stddef.h>
#include <stdint.h>

#define QEMU_SCMI_CMN_CTRL_PROTOCOL_ID   MOD_SCMI_PLATFORM_PROTOCOL_ID_MIN
#define QEMU_SCMI_CMN_CTRL_PROTOCOL_VER  UINT32_C(0x10000)

enum qemu_scmi_cmn_ctrl_message_id {
    QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_VERSION = MOD_SCMI_PROTOCOL_VERSION,
    QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_ATTRIBUTES =
        MOD_SCMI_PROTOCOL_ATTRIBUTES,
    QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_MESSAGE_ATTRIBUTES =
        MOD_SCMI_PROTOCOL_MESSAGE_ATTRIBUTES,
    QEMU_SCMI_CMN_CTRL_MESSAGE_START_CMN_INIT = 0x3,
    QEMU_SCMI_CMN_CTRL_MESSAGE_COUNT,
};

struct qemu_scmi_cmn_ctrl_protocol_attributes_p2a {
    int32_t status;
    uint32_t attributes;
};

struct qemu_scmi_cmn_ctrl_protocol_message_attributes_a2p {
    uint32_t message_id;
};

struct qemu_scmi_cmn_ctrl_protocol_message_attributes_p2a {
    int32_t status;
    uint32_t attributes;
};

static struct {
    const struct mod_scmi_from_protocol_api *scmi_api;
    const struct mod_cmn_cyprus_control_api *cmn_api;
} qemu_scmi_cmn_ctrl_ctx;

static const size_t payload_size_table[QEMU_SCMI_CMN_CTRL_MESSAGE_COUNT] = {
    [QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_VERSION] = 0,
    [QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_ATTRIBUTES] = 0,
    [QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_MESSAGE_ATTRIBUTES] =
        sizeof(struct qemu_scmi_cmn_ctrl_protocol_message_attributes_a2p),
    [QEMU_SCMI_CMN_CTRL_MESSAGE_START_CMN_INIT] = 0,
};

static int qemu_scmi_cmn_ctrl_protocol_version_handler(
    fwk_id_t service_id,
    const uint32_t *payload)
{
    const struct scmi_protocol_version_p2a response = {
        .status = SCMI_SUCCESS,
        .version = QEMU_SCMI_CMN_CTRL_PROTOCOL_VER,
    };

    (void)payload;

    return qemu_scmi_cmn_ctrl_ctx.scmi_api->respond(
        service_id, &response, sizeof(response));
}

static int qemu_scmi_cmn_ctrl_protocol_attributes_handler(
    fwk_id_t service_id,
    const uint32_t *payload)
{
    const struct qemu_scmi_cmn_ctrl_protocol_attributes_p2a response = {
        .status = SCMI_SUCCESS,
        .attributes = 0,
    };

    (void)payload;

    return qemu_scmi_cmn_ctrl_ctx.scmi_api->respond(
        service_id, &response, sizeof(response));
}

static int qemu_scmi_cmn_ctrl_protocol_message_attributes_handler(
    fwk_id_t service_id,
    const uint32_t *payload)
{
    const struct qemu_scmi_cmn_ctrl_protocol_message_attributes_a2p
        *parameters =
            (const struct qemu_scmi_cmn_ctrl_protocol_message_attributes_a2p *)
                payload;
    struct qemu_scmi_cmn_ctrl_protocol_message_attributes_p2a response = {
        .status = SCMI_SUCCESS,
        .attributes = 0,
    };

    if (parameters->message_id >= QEMU_SCMI_CMN_CTRL_MESSAGE_COUNT) {
        response.status = SCMI_NOT_FOUND;
    } else if ((parameters->message_id !=
                QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_VERSION) &&
               (parameters->message_id !=
                QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_ATTRIBUTES) &&
               (parameters->message_id !=
                QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_MESSAGE_ATTRIBUTES) &&
               (parameters->message_id !=
                QEMU_SCMI_CMN_CTRL_MESSAGE_START_CMN_INIT)) {
        response.status = SCMI_NOT_SUPPORTED;
    }

    return qemu_scmi_cmn_ctrl_ctx.scmi_api->respond(
        service_id,
        &response,
        (response.status == SCMI_SUCCESS) ? sizeof(response) :
                                            sizeof(response.status));
}

static int qemu_scmi_cmn_ctrl_start_cmn_init_handler(
    fwk_id_t service_id,
    const uint32_t *payload)
{
    int status;
    int32_t response_status;

    (void)payload;

    status = qemu_scmi_cmn_ctrl_ctx.cmn_api->start_setup();
    response_status = (status == FWK_SUCCESS) ? SCMI_SUCCESS :
                                               SCMI_GENERIC_ERROR;

    return qemu_scmi_cmn_ctrl_ctx.scmi_api->respond(
        service_id, &response_status, sizeof(response_status));
}

static handler_table_t handler_table[QEMU_SCMI_CMN_CTRL_MESSAGE_COUNT] = {
    [QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_VERSION] =
        qemu_scmi_cmn_ctrl_protocol_version_handler,
    [QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_ATTRIBUTES] =
        qemu_scmi_cmn_ctrl_protocol_attributes_handler,
    [QEMU_SCMI_CMN_CTRL_MESSAGE_PROTOCOL_MESSAGE_ATTRIBUTES] =
        qemu_scmi_cmn_ctrl_protocol_message_attributes_handler,
    [QEMU_SCMI_CMN_CTRL_MESSAGE_START_CMN_INIT] =
        qemu_scmi_cmn_ctrl_start_cmn_init_handler,
};

static int qemu_scmi_cmn_ctrl_get_scmi_protocol_id(
    fwk_id_t protocol_id,
    uint8_t *scmi_protocol_id)
{
    (void)protocol_id;

    *scmi_protocol_id = (uint8_t)QEMU_SCMI_CMN_CTRL_PROTOCOL_ID;
    return FWK_SUCCESS;
}

static int qemu_scmi_cmn_ctrl_message_handler(
    fwk_id_t protocol_id,
    fwk_id_t service_id,
    const uint32_t *payload,
    size_t payload_size,
    unsigned int message_id)
{
    int validation_result;

    (void)protocol_id;

    validation_result = qemu_scmi_cmn_ctrl_ctx.scmi_api->scmi_message_validation(
        QEMU_SCMI_CMN_CTRL_PROTOCOL_ID,
        service_id,
        payload,
        payload_size,
        message_id,
        payload_size_table,
        QEMU_SCMI_CMN_CTRL_MESSAGE_COUNT,
        handler_table);
    if (validation_result != SCMI_SUCCESS) {
        return qemu_scmi_cmn_ctrl_ctx.scmi_api->respond(
            service_id, &validation_result, sizeof(validation_result));
    }

    return handler_table[message_id](service_id, payload);
}

static struct mod_scmi_to_protocol_api qemu_scmi_cmn_ctrl_scmi_api = {
    .get_scmi_protocol_id = qemu_scmi_cmn_ctrl_get_scmi_protocol_id,
    .message_handler = qemu_scmi_cmn_ctrl_message_handler,
};

static int qemu_scmi_cmn_ctrl_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    (void)module_id;
    (void)element_count;
    (void)data;

    return FWK_SUCCESS;
}

static int qemu_scmi_cmn_ctrl_bind(fwk_id_t id, unsigned int round)
{
    int status;

    (void)id;

    if (round != 0) {
        return FWK_SUCCESS;
    }

    status = fwk_module_bind(
        FWK_ID_MODULE(FWK_MODULE_IDX_SCMI),
        FWK_ID_API(FWK_MODULE_IDX_SCMI, MOD_SCMI_API_IDX_PROTOCOL),
        &qemu_scmi_cmn_ctrl_ctx.scmi_api);
    if (status != FWK_SUCCESS) {
        return status;
    }

    return fwk_module_bind(
        FWK_ID_MODULE(FWK_MODULE_IDX_CMN_CYPRUS),
        FWK_ID_API(
            FWK_MODULE_IDX_CMN_CYPRUS, MOD_CMN_CYPRUS_API_IDX_CONTROL),
        &qemu_scmi_cmn_ctrl_ctx.cmn_api);
}

static int qemu_scmi_cmn_ctrl_process_bind_request(
    fwk_id_t source_id,
    fwk_id_t target_id,
    fwk_id_t api_id,
    const void **api)
{
    (void)target_id;
    (void)api_id;

    if (!fwk_id_is_equal(source_id, FWK_ID_MODULE(FWK_MODULE_IDX_SCMI))) {
        return FWK_E_ACCESS;
    }

    *api = &qemu_scmi_cmn_ctrl_scmi_api;
    return FWK_SUCCESS;
}

const struct fwk_module_config config_qemu_scmi_cmn_ctrl = { 0 };

const struct fwk_module module_qemu_scmi_cmn_ctrl = {
    .type = FWK_MODULE_TYPE_PROTOCOL,
    .api_count = 1,
    .init = qemu_scmi_cmn_ctrl_init,
    .bind = qemu_scmi_cmn_ctrl_bind,
    .process_bind_request = qemu_scmi_cmn_ctrl_process_bind_request,
};
