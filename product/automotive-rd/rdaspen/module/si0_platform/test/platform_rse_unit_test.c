/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "unity.h"

#include <Mockfwk_id.h>
#include <Mockfwk_module.h>

#include <mod_si0_platform.h>

#include <fwk_module_idx.h>

#include UNIT_TEST_SRC

#include "config_si0_platform.h"

static int interrupt_status;

static int trigger_interrupt(fwk_id_t transport_id)
{
    return interrupt_status;
}

static int release_transport_channel_lock(fwk_id_t transport_id)
{
    return FWK_SUCCESS;
}

static struct mod_transport_firmware_api mock_transport_api = {
    .trigger_interrupt = trigger_interrupt,
    .release_transport_channel_lock = release_transport_channel_lock,
};

int wait(
    fwk_id_t dev_id,
    uint32_t microseconds,
    bool (*cond)(void *),
    void *data)
{
    return FWK_SUCCESS;
}

static struct mod_timer_api mock_timer_api = {
    .wait = wait,
};

void setUp(void)
{
    ctx.transport_api = &mock_transport_api;
    ctx.timer_api = &mock_timer_api;
    interrupt_status = FWK_SUCCESS;
}

void tearDown(void)
{
    /* Do Nothing */
}

/*!
 * \brief platform_rse unit test: platform_rse_bind(),
 *
 *  \details Test successful bind
 */
void test_platform_rse_bind_success(void)
{
    int status;

    fwk_module_bind_IgnoreAndReturn(FWK_SUCCESS);

    status = platform_rse_bind(&platform_config_data);

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

/*!
 * \brief platform_rse unit test: platform_rse_bind(),
 *
 *  \details Test failure in bind
 */
void test_platform_rse_bind_fail(void)
{
    int status;

    fwk_module_bind_IgnoreAndReturn(FWK_E_DATA);

    status = platform_rse_bind(&platform_config_data);

    TEST_ASSERT_EQUAL(status, FWK_E_DATA);
}

/*!
 * \brief platform_rse unit test: get_platform_transport_signal_api(),
 *
 *  \details Test get_platform_transport_signal_api
 */
void test_get_platform_transport_signal_api(void)
{
    const void *api = get_platform_transport_signal_api();
    TEST_ASSERT_EQUAL(api, &platform_transport_signal_api);
}

/*!
 * \brief platform_rse unit test: notify_rse_and_wait_for_response(),
 *
 *  \details Test successful call to notify_rse_and_wait_for_response
 */
void test_notify_rse_and_wait_for_response_success(void)
{
    int status;

    signal_message(fwk_module_id_transport);
    status = notify_rse_and_wait_for_response();

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

/*!
 * \brief platform_rse unit test: notify_rse_and_wait_for_response(),
 *
 *  \details Test failure in call to notify_rse_and_wait_for_response
 */
void test_notify_rse_and_wait_for_response_fail(void)
{
    int status;

    interrupt_status = FWK_E_BUSY;

    signal_message(fwk_module_id_transport);
    status = notify_rse_and_wait_for_response();

    TEST_ASSERT_EQUAL(status, FWK_E_BUSY);
}

int platform_rse_test_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_platform_rse_bind_success);
    RUN_TEST(test_platform_rse_bind_fail);
    RUN_TEST(test_get_platform_transport_signal_api);
    RUN_TEST(test_notify_rse_and_wait_for_response_success);
    RUN_TEST(test_notify_rse_and_wait_for_response_fail);

    return UNITY_END();
}

int main(void)
{
    return platform_rse_test_main();
}
