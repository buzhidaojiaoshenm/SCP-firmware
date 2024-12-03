/*
 * Arm SCP/MCP Software
 * Copyright (c) 2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Power capping module unit test.
 */

#include "config_power_distributor.h"
#include "scp_unity.h"
#include "unity.h"

#include <Mockfwk_mm.h>
#include <Mockfwk_module.h>

#include <mod_power_distributor.h>

#include UNIT_TEST_SRC

struct mod_power_distributor_domain_ctx test_ctx_table[TEST_DOMAIN_COUNT];

struct interface_power_management_api test_power_management_api = { 0 };

void setUp(void)
{
    memset((void *)test_ctx_table, 0U, sizeof(test_ctx_table));
    power_distributor_ctx.domain = test_ctx_table;

    power_distributor_ctx.domain_count = TEST_DOMAIN_COUNT;
    for (unsigned int i = 0U; i < TEST_DOMAIN_COUNT; i++) {
        power_distributor_ctx.domain[i].controller_api =
            &test_power_management_api;
        power_distributor_ctx.domain[i].config =
            &test_power_distibutor_domain_config[i];
    }
}

void utest_power_distributor_init_success(void)
{
    int status;
    unsigned int element_count = 23U;
    struct mod_power_distributor_domain_ctx domain = { 0 };
    power_distributor_ctx.domain_count = 0;

    fwk_mm_calloc_ExpectAndReturn(
        element_count, sizeof(power_distributor_ctx.domain[0]), &domain);

    status = power_distributor_init(
        FWK_ID_MODULE(FWK_MODULE_IDX_POWER_DISTRIBUTOR), element_count, NULL);
    TEST_ASSERT_EQUAL(element_count, power_distributor_ctx.domain_count);
    TEST_ASSERT_EQUAL(&domain, power_distributor_ctx.domain);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
}

void utest_power_distributor_element_init_success(void)
{
    int status;
    fwk_id_t element_id;
    static struct mod_power_distibutor_domain_config
        domains_config[TEST_DOMAIN_COUNT] = { 0 };

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        element_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_POWER_DISTRIBUTOR, index);

        status = power_distributor_element_init(
            element_id, 30U, &domains_config[index]);

        TEST_ASSERT_EQUAL_PTR(
            &domains_config[index], power_distributor_ctx.domain[index].config);
        TEST_ASSERT_EQUAL_PTR(
            NULL, power_distributor_ctx.domain[index].controller_api);
        TEST_ASSERT_EQUAL_PTR(
            NULL, power_distributor_ctx.domain[index].node.children_idx_table);
        TEST_ASSERT_EQUAL(
            0, power_distributor_ctx.domain[index].node.children_count);
        TEST_ASSERT_EQUAL(
            0, power_distributor_ctx.domain[index].node.data.power_demand);
        TEST_ASSERT_EQUAL(
            0, power_distributor_ctx.domain[index].node.data.power_budget);
        TEST_ASSERT_EQUAL(
            NO_POWER_LIMIT,
            power_distributor_ctx.domain[index].node.data.power_limit);
        TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    }
}

void utest_mod_distributor_bind_round_0(void)
{
    int status;
    unsigned int round = 0U;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        fwk_id_t domain_id =
            FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_DISTRIBUTOR, index);
        struct mod_power_distributor_domain_ctx *domain_ctx =
            &(power_distributor_ctx.domain[index]);

        if (!fwk_id_is_equal(
                domain_ctx->config->controller_api_id, FWK_ID_NONE)) {
            fwk_module_bind_ExpectAndReturn(
                fwk_id_build_module_id(domain_ctx->config->controller_api_id),
                domain_ctx->config->controller_api_id,
                &domain_ctx->controller_api,
                FWK_SUCCESS);
        }

        status = power_distributor_bind(domain_id, round);

        TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    }
}

void utest_mod_distributor_bind_invalid_round(void)
{
    int status;
    unsigned int round = 1U;

    for (unsigned int index = 0U; index < TEST_DOMAIN_COUNT; index++) {
        fwk_id_t domain_id =
            FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_DISTRIBUTOR, index);

        status = power_distributor_bind(domain_id, round);

        TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    }
}

void utest_mod_distributor_process_bind_request(void)
{
    int status;
    fwk_id_t dummy_id = FWK_ID_NONE;
    const void *api = NULL;

    status = power_distributor_process_bind_request(
        dummy_id,
        dummy_id,
        FWK_ID_API(
            FWK_MODULE_IDX_POWER_DISTRIBUTOR,
            MOD_POWER_DISTRIBUTOR_API_IDX_DISTRIBUTION),
        &api);

    TEST_ASSERT_EQUAL_PTR(&distribution_api, api);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
}

void utest_mod_distributor_process_bind_request_invalid_api(void)
{
    int status;
    fwk_id_t dummy_id = FWK_ID_NONE;
    enum mod_power_distributor_api_idx invalid_api_id =
        MOD_POWER_DISTRIBUTOR_API_IDX_COUNT;
    const void *api = NULL;

    status = power_distributor_process_bind_request(
        dummy_id,
        dummy_id,
        FWK_ID_API(FWK_MODULE_IDX_POWER_DISTRIBUTOR, invalid_api_id),
        &api);

    TEST_ASSERT_EQUAL(FWK_E_PARAM, status);
}

void utest_mod_distributor_process_bind_request_invalid_module(void)
{
    int status;
    fwk_id_t dummy_id = FWK_ID_NONE;
    enum fwk_module_idx invalid_module_idx = FWK_MODULE_IDX_COUNT;
    const void *api = NULL;

    status = power_distributor_process_bind_request(
        dummy_id,
        dummy_id,
        FWK_ID_API(
            invalid_module_idx, MOD_POWER_DISTRIBUTOR_API_IDX_DISTRIBUTION),
        &api);

    TEST_ASSERT_EQUAL(FWK_E_PARAM, status);
}

void utest_mod_distributor_post_init_success(void)
{
    int status = FWK_E_DATA;
    struct mod_power_distibutor_domain_config config[TEST_DOMAIN_COUNT] = {
        [TEST_DOMAIN_SOC] = { .parent_idx = TEST_DOMAIN_NONE,},
        [TEST_DOMAIN_CPU] = { .parent_idx = TEST_DOMAIN_SOC, },
        [TEST_DOMAIN_GPU] = { .parent_idx = TEST_DOMAIN_SOC, },
        [TEST_DOMAIN_CPU_BIG] = { .parent_idx = TEST_DOMAIN_CPU, },
        [TEST_DOMAIN_CPU_LITTLE] = { .parent_idx = TEST_DOMAIN_CPU, },
    };
    size_t children_of_soc[2];
    size_t children_of_cpu[2];
    power_distributor_ctx.domain_count = TEST_DOMAIN_COUNT;

    for (size_t i = 0; i < TEST_DOMAIN_COUNT; ++i) {
        power_distributor_ctx.domain[i].config = &config[i];
    }

    fwk_mm_calloc_ExpectAndReturn(
        2, sizeof(children_of_soc[0]), &children_of_soc);
    fwk_mm_calloc_ExpectAndReturn(
        2, sizeof(children_of_cpu[0]), &children_of_cpu);

    status = power_distributor_post_init(
        FWK_ID_MODULE(FWK_MODULE_IDX_POWER_DISTRIBUTOR));

    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL_PTR(
        &children_of_soc,
        power_distributor_ctx.domain[TEST_DOMAIN_SOC].node.children_idx_table);
    TEST_ASSERT_EQUAL(
        2, power_distributor_ctx.domain[TEST_DOMAIN_SOC].node.children_count);
    TEST_ASSERT_EQUAL_PTR(
        &children_of_cpu,
        power_distributor_ctx.domain[TEST_DOMAIN_CPU].node.children_idx_table);
    TEST_ASSERT_EQUAL(
        2, power_distributor_ctx.domain[TEST_DOMAIN_CPU].node.children_count);
    TEST_ASSERT_EQUAL_PTR(
        NULL,
        power_distributor_ctx.domain[TEST_DOMAIN_GPU].node.children_idx_table);
    TEST_ASSERT_EQUAL(
        0, power_distributor_ctx.domain[TEST_DOMAIN_GPU].node.children_count);
    TEST_ASSERT_EQUAL_PTR(
        NULL,
        power_distributor_ctx.domain[TEST_DOMAIN_CPU_BIG]
            .node.children_idx_table);
    TEST_ASSERT_EQUAL(
        0,
        power_distributor_ctx.domain[TEST_DOMAIN_CPU_BIG].node.children_count);
    TEST_ASSERT_EQUAL_PTR(
        NULL,
        power_distributor_ctx.domain[TEST_DOMAIN_CPU_LITTLE]
            .node.children_idx_table);
    TEST_ASSERT_EQUAL(
        0,
        power_distributor_ctx.domain[TEST_DOMAIN_CPU_LITTLE]
            .node.children_count);
}

void tearDown(void)
{
}

int power_distributor_test_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(utest_power_distributor_init_success);
    RUN_TEST(utest_power_distributor_element_init_success);
    RUN_TEST(utest_mod_distributor_bind_round_0);
    RUN_TEST(utest_mod_distributor_bind_invalid_round);
    RUN_TEST(utest_mod_distributor_process_bind_request);
    RUN_TEST(utest_mod_distributor_process_bind_request_invalid_api);
    RUN_TEST(utest_mod_distributor_process_bind_request_invalid_module);
    RUN_TEST(utest_mod_distributor_post_init_success);

    return UNITY_END();
}

#if !defined(TEST_ON_TARGET)
int main(void)
{
    return power_distributor_test_main();
}
#endif
