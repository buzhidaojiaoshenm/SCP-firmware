/*
 * Arm SCP/MCP Software
 * Copyright (c) 2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Power capping module unit test.
 */

#include "config_power_capping.h"
#include "mod_power_capping_extra.h"
#include "scp_unity.h"
#include "unity.h"

#include <Mockfwk_core.h>
#include <Mockfwk_mm.h>
#include <Mockfwk_module.h>
#include <Mockfwk_notification.h>
#include <Mockmod_power_capping_extra.h>
#include <internal/Mockfwk_core_internal.h>

#include <mod_power_capping.h>

#include <fwk_module_idx.h>

#include UNIT_TEST_SRC

struct pcapping_domain_ctx test_ctx_table[TEST_DOMAIN_COUNT];

struct interface_power_management_api test_power_management_api = {
    .get_power_limit = get_power_limit,
};

void setUp(void)
{
    memset((void *)test_ctx_table, 0U, sizeof(test_ctx_table));

    pcapping_domain_ctx_table = test_ctx_table;

    pcapping_ctx.domain_count = TEST_DOMAIN_COUNT;

    for (unsigned int i = 0U; i < TEST_DOMAIN_COUNT; i++) {
        pcapping_domain_ctx_table[i].power_management_api =
            &test_power_management_api;
        pcapping_domain_ctx_table[i].config =
            (struct mod_power_capping_domain_config *)test_domain_config[i]
                .data;
    }
}

void tearDown(void)
{
    Mockmod_power_capping_extra_Verify();
    Mockmod_power_capping_extra_Destroy();
}

void utest_mod_pcapping_request_cap_greater_than_applied(void)
{
    int status;
    fwk_id_t domain_id;
    uint32_t requested_cap = 25U;
    uint32_t applied_cap = 24U;
    struct pcapping_domain_ctx *domain_ctx;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        domain_ctx = &pcapping_domain_ctx_table[index];

        domain_ctx->applied_cap = applied_cap;

        status = mod_pcapping_request_cap(domain_id, requested_cap);

        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
        TEST_ASSERT_EQUAL(domain_ctx->applied_cap, applied_cap);
        TEST_ASSERT_EQUAL(domain_ctx->requested_cap, requested_cap);
    }
}

void utest_mod_pcapping_request_cap_equal_applied(void)
{
    int status;
    fwk_id_t domain_id;
    uint32_t applied_cap = 50;
    struct pcapping_domain_ctx *domain_ctx;
    uint32_t requested_cap = applied_cap;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        domain_ctx = &pcapping_domain_ctx_table[index];

        domain_ctx->applied_cap = applied_cap;

        status = mod_pcapping_request_cap(domain_id, requested_cap);

        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
        TEST_ASSERT_EQUAL(domain_ctx->applied_cap, applied_cap);
        TEST_ASSERT_EQUAL(domain_ctx->requested_cap, requested_cap);
    }
}

void utest_mod_pcapping_request_cap_smaller_than_applied(void)
{
    int status;
    fwk_id_t domain_id;
    struct pcapping_domain_ctx *domain_ctx;
    uint32_t requested_cap = 23U;
    uint32_t applied_cap = 24U;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        domain_ctx = &pcapping_domain_ctx_table[index];

        domain_ctx->applied_cap = applied_cap;

        status = mod_pcapping_request_cap(domain_id, requested_cap);

        TEST_ASSERT_EQUAL(status, FWK_PENDING);
        TEST_ASSERT_EQUAL(domain_ctx->applied_cap, applied_cap);
        TEST_ASSERT_EQUAL(domain_ctx->requested_cap, requested_cap);
    }
}

void utest_mod_pcapping_request_cap_e_param(void)
{
    int status;
    fwk_id_t domain_id =
        FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_CAPPING, TEST_DOMAIN_COUNT);

    status = mod_pcapping_request_cap(domain_id, 0U);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mod_pcapping_get_applied_cap_success(void)
{
    int status;
    fwk_id_t domain_id;
    struct pcapping_domain_ctx *domain_ctx;
    uint32_t cap;
    uint32_t applied_cap = 34U;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        domain_ctx = &pcapping_domain_ctx_table[index];

        domain_ctx->applied_cap = applied_cap;

        status = mod_pcapping_get_applied_cap(domain_id, &cap);
        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
        TEST_ASSERT_EQUAL(applied_cap, cap);
    }
}

void utest_mod_pcapping_get_applied_cap_e_param(void)
{
    int status;
    uint32_t cap;

    fwk_id_t domain_id =
        FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_CAPPING, TEST_DOMAIN_COUNT);

    status = mod_pcapping_get_applied_cap(domain_id, &cap);

    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mod_pcapping_get_power_limit_success(void)
{
    int status;
    fwk_id_t domain_id;
    struct pcapping_domain_ctx *domain_ctx;
    uint32_t limit;
    uint32_t requested_cap = 40U;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        domain_ctx = &pcapping_domain_ctx_table[index];

        domain_ctx->requested_cap = requested_cap;

        status = mod_pcapping_get_power_limit(domain_id, &limit);
        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
        TEST_ASSERT_EQUAL(requested_cap, limit);
    }
}

void utest_mod_pcapping_get_power_limit_e_param(void)
{
    int status;
    uint32_t limit;

    fwk_id_t domain_id =
        FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_CAPPING, TEST_DOMAIN_COUNT);

    status = mod_pcapping_get_power_limit(domain_id, &limit);

    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_pcapping_init_success(void)
{
    int status;
    unsigned int element_count = 23U;
    void *calloc_ret = (void *)1234567;

    fwk_mm_calloc_ExpectAnyArgsAndReturn(calloc_ret);

    status = pcapping_init(
        FWK_ID_MODULE(FWK_MODULE_IDX_POWER_CAPPING), element_count, NULL);
    TEST_ASSERT_EQUAL(element_count, pcapping_ctx.domain_count);
    TEST_ASSERT_EQUAL(calloc_ret, pcapping_domain_ctx_table);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_pcapping_domain_init_success(void)
{
    int status;
    fwk_id_t element_id;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        element_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        void *data = (void *)(uintptr_t)(345 + index);

        status = pcapping_domain_init(element_id, 30U, data);

        TEST_ASSERT_EQUAL_PTR(data, pcapping_domain_ctx_table[index].config);
        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    }
}

void utest_mod_pcapping_process_notification_success(void)
{
    int status;
    fwk_id_t domain_id;
    struct pcapping_domain_ctx *domain_ctx;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_CAPPING, index);
        domain_ctx = &(pcapping_domain_ctx_table[index]);

        struct fwk_event notification = {
            .id = domain_ctx->config->power_limit_set_notification_id,
            .target_id = domain_id,
        };

        uint32_t applied_cap = 44U + index;
        get_power_limit_ExpectAndReturn(
            domain_ctx->config->power_limiter_id, NULL, FWK_SUCCESS);
        get_power_limit_IgnoreArg_power_limit();
        get_power_limit_ReturnThruPtr_power_limit(&applied_cap);
        struct fwk_event outbound_notification = {
            .source_id = domain_id,
            .id = FWK_ID_NOTIFICATION_INIT(
                FWK_MODULE_IDX_POWER_CAPPING,
                MOD_POWER_CAPPING_NOTIFICATION_IDX_CAP_CHANGE),
        };
        fwk_notification_notify_ExpectWithArrayAndReturn(
            &outbound_notification,
            1U,
            &(domain_ctx->notifications_sent_count),
            1U,
            FWK_SUCCESS);

        status = mod_pcapping_process_notification(&notification, NULL);

        TEST_ASSERT_EQUAL(domain_ctx->applied_cap, applied_cap);
        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    }
}

void utest_mod_pcapping_process_notification_e_param(void)
{
    int status;
    struct fwk_event notif_event = {
        .target_id = FWK_ID_ELEMENT_INIT(
            FWK_MODULE_IDX_POWER_CAPPING, TEST_DOMAIN_COUNT),
    };

    status = mod_pcapping_process_notification(&notif_event, NULL);

    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mod_pcapping_bind_round_0(void)
{
    int status;
    unsigned int round = 0U;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        fwk_id_t domain_id =
            FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_CAPPING, index);
        struct pcapping_domain_ctx *domain_ctx =
            &(pcapping_domain_ctx_table[index]);

        fwk_module_bind_ExpectAndReturn(
            domain_ctx->config->power_limiter_id,
            domain_ctx->config->power_limiter_api_id,
            &domain_ctx->power_management_api,
            FWK_SUCCESS);

        status = mod_pcapping_bind(domain_id, round);

        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    }
}

void utest_mod_pcapping_bind_round_not_0(void)
{
    int status;
    unsigned int round = 1U;
    fwk_id_t domain_id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_CAPPING, 0);

    status = mod_pcapping_bind(domain_id, round);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mod_pcapping_bind_module(void)
{
    int status;
    unsigned int round = 0U;
    fwk_id_t domain_id = FWK_ID_MODULE_INIT(FWK_MODULE_IDX_POWER_CAPPING);

    status = mod_pcapping_bind(domain_id, round);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mod_pcapping_start_element(void)
{
    int status;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        fwk_id_t domain_id =
            FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_CAPPING, index);
        struct pcapping_domain_ctx *domain_ctx =
            &(pcapping_domain_ctx_table[index]);

        fwk_notification_subscribe_ExpectAndReturn(
            domain_ctx->config->power_limit_set_notification_id,
            domain_ctx->config->power_limit_set_notifier_id,
            domain_id,
            FWK_SUCCESS);
        status = mod_pcapping_start(domain_id);

        TEST_ASSERT_EQUAL(domain_ctx->requested_cap, UINT32_MAX);
        TEST_ASSERT_EQUAL(domain_ctx->applied_cap, UINT32_MAX);
        TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    }
}

void utest_mod_pcapping_start_module(void)
{
    int status;
    fwk_id_t domain_id = FWK_ID_MODULE_INIT(FWK_MODULE_IDX_POWER_CAPPING);

    status = mod_pcapping_start(domain_id);

    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mod_pcapping_process_bind_request_cap_api(void)
{
    int status;
    fwk_id_t dummy_id = FWK_ID_NONE;
    const void *api;

    status = mod_pcapping_process_bind_request(
        dummy_id,
        dummy_id,
        FWK_ID_API(FWK_MODULE_IDX_POWER_CAPPING, MOD_POWER_CAPPING_API_IDX_CAP),
        &api);

    TEST_ASSERT_EQUAL_PTR(api, &pcapping_api);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mod_pcapping_process_bind_request_power_management_api(void)
{
    int status;
    fwk_id_t dummy_id = FWK_ID_NONE;
    const void *api;

    status = mod_pcapping_process_bind_request(
        dummy_id,
        dummy_id,
        FWK_ID_API(
            FWK_MODULE_IDX_POWER_CAPPING,
            MOD_POWER_CAPPING_API_IDX_POWER_MANAGEMENT),
        &api);

    TEST_ASSERT_EQUAL_PTR(api, &power_management_api);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mod_pcapping_process_bind_request_e_param(void)
{
    int status;
    fwk_id_t dummy_id = FWK_ID_NONE;
    const void *api;

    status = mod_pcapping_process_bind_request(
        dummy_id,
        dummy_id,
        FWK_ID_API(
            FWK_MODULE_IDX_POWER_CAPPING, MOD_POWER_CAPPING_API_IDX_COUNT),
        &api);

    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

int power_capping_test_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(utest_mod_pcapping_request_cap_greater_than_applied);
    RUN_TEST(utest_mod_pcapping_request_cap_equal_applied);
    RUN_TEST(utest_mod_pcapping_request_cap_smaller_than_applied);
    RUN_TEST(utest_mod_pcapping_request_cap_e_param);
    RUN_TEST(utest_mod_pcapping_get_applied_cap_success);
    RUN_TEST(utest_mod_pcapping_get_applied_cap_e_param);
    RUN_TEST(utest_mod_pcapping_get_power_limit_success);
    RUN_TEST(utest_mod_pcapping_get_power_limit_e_param);
    RUN_TEST(utest_pcapping_init_success);
    RUN_TEST(utest_pcapping_domain_init_success);
    RUN_TEST(utest_mod_pcapping_process_notification_success);
    RUN_TEST(utest_mod_pcapping_process_notification_e_param);
    RUN_TEST(utest_mod_pcapping_bind_round_0);
    RUN_TEST(utest_mod_pcapping_bind_round_not_0);
    RUN_TEST(utest_mod_pcapping_bind_module);
    RUN_TEST(utest_mod_pcapping_start_element);
    RUN_TEST(utest_mod_pcapping_start_module);
    RUN_TEST(utest_mod_pcapping_process_bind_request_cap_api);
    RUN_TEST(utest_mod_pcapping_process_bind_request_power_management_api);
    RUN_TEST(utest_mod_pcapping_process_bind_request_e_param);

    return UNITY_END();
}

#if !defined(TEST_ON_TARGET)
int main(void)
{
    return power_capping_test_main();
}
#endif
