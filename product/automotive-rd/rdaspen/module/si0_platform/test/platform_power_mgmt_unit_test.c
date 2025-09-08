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

int set_state_status;

static int set_state(
    fwk_id_t core_idx,
    bool response_requested,
    unsigned int state)
{
    return set_state_status;
}

static struct mod_pd_restricted_api pd_api = {
    .set_state = set_state,
};

void setUp(void)
{
    pd_restricted_api = &pd_api;
    set_state_status = FWK_SUCCESS;
}

void tearDown(void)
{
    /* Do Nothing */
}

/*!
 * \brief platform_power_mgmt unit test: platform_power_mgmt_bind(),
 *
 *  \details Test successful bind
 */
void test_platform_power_mgmt_bind_success(void)
{
    int status;

    fwk_module_bind_IgnoreAndReturn(FWK_SUCCESS);

    status = platform_power_mgmt_bind();

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

/*!
 * \brief platform_power_mgmt unit test: platform_power_mgmt_bind(),
 *
 *  \details Test failure in bind
 */
void test_platform_power_mgmt_bind_fail(void)
{
    int status;

    fwk_module_bind_IgnoreAndReturn(FWK_E_DATA);

    status = platform_power_mgmt_bind();

    TEST_ASSERT_EQUAL(status, FWK_E_DATA);
}

/*!
 * \brief platform_power_mgmt unit test: init_ap(),
 *
 *  \details Test successful call to init_ap
 */
void test_init_ap_success(void)
{
    int status;

    status = init_ap();

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

/*!
 * \brief platform_power_mgmt unit test: init_ap(),
 *
 *  \details Test failure in call to init_ap
 */
void test_init_ap_fail(void)
{
    int status;

    set_state_status = FWK_E_BUSY;
    status = init_ap();

    TEST_ASSERT_EQUAL(status, FWK_E_BUSY);
}

int platform_power_mgmt_test_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_platform_power_mgmt_bind_success);
    RUN_TEST(test_platform_power_mgmt_bind_fail);
    RUN_TEST(test_init_ap_success);
    RUN_TEST(test_init_ap_fail);

    return UNITY_END();
}

int main(void)
{
    return platform_power_mgmt_test_main();
}
