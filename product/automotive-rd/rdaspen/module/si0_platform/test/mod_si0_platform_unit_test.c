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

void setUp(void)
{
    /* Do Nothing */
}

void tearDown(void)
{
    /* Do Nothing */
}

/*!
 * \brief SI0 Platform unit test: si0_platform_mod_init(),
 *
 *  \details Test successful initialization of si0_platform module
 */
void test_si0_platform_mod_init_success(void)
{
    int status;

    status = si0_platform_mod_init(fwk_module_id_si0_platform, 0, NULL);

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

/*!
 * \brief SI0 Platform unit test: si0_platform_bind(),
 *
 *  \details Test successful bind of si0_platform module
 */
void test_si0_platform_bind_success(void)
{
    int status;

    status = si0_platform_bind(fwk_module_id_si0_platform, 0);

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

/*!
 * \brief SI0 Platform unit test: si0_platform_start(),
 *
 *  \details Test successful start of si0_platform module
 */
void test_si0_platform_mod_start_success(void)
{
    int status;

    status = si0_platform_start(fwk_module_id_si0_platform);

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

int si0_platform_test_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_si0_platform_mod_init_success);
    RUN_TEST(test_si0_platform_bind_success);
    RUN_TEST(test_si0_platform_mod_start_success);

    return UNITY_END();
}

int main(void)
{
    return si0_platform_test_main();
}
