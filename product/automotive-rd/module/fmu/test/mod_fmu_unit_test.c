/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "scp_unity.h"
#include "unity.h"

#include <Mockfwk_interrupt.h>
#include <Mockfwk_module.h>
#include <Mockfwk_notification.h>

#include <fwk_macros.h>
#include <fwk_mm.h>
#include <fwk_string.h>

#include UNIT_TEST_SRC
#include <config_fmu.h>
#include <internal/fmu_reg.h>

#include <mod_fmu.h>

void setUp(void)
{
    unsigned int idx;
    /* Clear the cluster control registers between tests */
    fwk_str_memset(fmu_reg, 0, sizeof(fmu_reg));

    ctx.num_devices = SCP_FMU_COUNT;
    ctx.device_config = fwk_mm_calloc(SCP_FMU_COUNT, sizeof(uintptr_t));
    for (idx = 0; idx < SCP_FMU_COUNT; idx++) {
        ctx.device_config[idx] = fmu_devices[idx].data;
    }
}

void tearDown(void)
{
    /* Do nothing */
}

void test_fmu_init(void)
{
    int status;

    status = fmu_init(fwk_module_id_fmu, SCP_FMU_COUNT, config_fmu.data);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    TEST_ASSERT_EQUAL(SCP_FMU_COUNT, ctx.num_devices);
}

void test_fmu_device_init(void)
{
    int status;
    unsigned int idx;
    fwk_id_t element_id;

    for (idx = 0; idx < SCP_FMU_COUNT; idx++) {
        element_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_FMU, idx);
        status = fmu_device_init(element_id, 0, fmu_devices[idx].data);
        TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

        TEST_ASSERT_EQUAL(fmu_devices[idx].data, ctx.device_config[idx]);
    }
}

void test_fmu_start(void)
{
    int status;

    fwk_module_get_data_ExpectAndReturn(fwk_module_id_fmu, config_fmu.data);

    fwk_interrupt_set_isr_ExpectAndReturn(12, fmu_isr_critical, FWK_SUCCESS);
    fwk_interrupt_set_isr_ExpectAndReturn(
        13, fmu_isr_non_critical, FWK_SUCCESS);
    fwk_interrupt_enable_ExpectAndReturn(12, FWK_SUCCESS);
    fwk_interrupt_enable_ExpectAndReturn(13, FWK_SUCCESS);

    status = fmu_start(fwk_module_id_fmu);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
}

void test_fmu_process_bind_request(void)
{
    int status;
    uintptr_t api;

    status = fmu_process_bind_request(
        fwk_module_id_fake,
        fwk_module_id_fmu,
        FWK_ID_API(FWK_MODULE_IDX_FMU, MOD_FMU_DEVICE_API_IDX),
        (void *)&api);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(&fmu_api, api);
}

void test_fmu_next_fault_simple(void)
{
    int status;
    uint32_t val;
    struct mod_fmu_fault fault = {
        .device_idx = 0,
        .node_idx = 1,
        .sm_idx = MOD_FMU_SM_SYSTEM_INPUT_ERROR,
    };
    struct fwk_event event = { 0 };
    struct mod_fmu_fault_notification_params *params;

    status = fmu_api.inject(&fault);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    /* Asert fault has been injected state */
    val = fwk_mmio_read_32(
        (uintptr_t)fmu_reg[SCP_FMU_ROOT] + FMU_FIELD_ERRIMPDEF(1));
    TEST_ASSERT_BITS(
        FMU_ERRIMPDEF_IE_MASK,
        MOD_FMU_SM_SYSTEM_INPUT_ERROR << FMU_ERRIMPDEF_IE_SHIFT,
        val);

    /* Set up FMU fault response */
    fwk_mmio_write_32(
        (uintptr_t)fmu_reg[SCP_FMU_ROOT] + FMU_FIELD_ERRGSR_L(0), FWK_BIT(1));
    fwk_mmio_write_32(
        (uintptr_t)fmu_reg[SCP_FMU_ROOT] + FMU_FIELD_ERR_STATUS(1),
        MOD_FMU_SM_SYSTEM_INPUT_ERROR << FMU_ERR_STATUS_IERR_SHIFT);

    event.id = mod_fmu_notification_id_fault;
    event.source_id = fwk_module_id_fmu;
    params = (struct mod_fmu_fault_notification_params *)event.params;
    params->critical = false;
    params->fault.device_idx = 0;
    params->fault.node_idx = 1;
    params->fault.sm_idx = MOD_FMU_SM_SYSTEM_INPUT_ERROR;
    fwk_notification_notify_ExpectAndReturn(&event, NULL, FWK_SUCCESS);
    fwk_notification_notify_IgnoreArg_count();
    TEST_ASSERT_TRUE(next_fault(false));
}

void test_fmu_next_fault_upstream(void)
{
    int status;
    uint32_t val;
    struct mod_fmu_fault fault = {
        .device_idx = 1,
        .node_idx = 0,
        .sm_idx = MOD_FMU_SM_SYSTEM_INPUT_ERROR,
    };
    struct fwk_event event = { 0 };
    struct mod_fmu_fault_notification_params *params;

    status = fmu_api.inject(&fault);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    /* Asert fault has been injected state */
    val = fwk_mmio_read_32(
        (uintptr_t)fmu_reg[SCP_FMU_1] + FMU_FIELD_ERRIMPDEF(0));
    TEST_ASSERT_BITS(
        FMU_ERRIMPDEF_IE_MASK,
        MOD_FMU_SM_SYSTEM_INPUT_ERROR << FMU_ERRIMPDEF_IE_SHIFT,
        val);

    /* Set up FMU fault response */
    fwk_mmio_write_32(
        (uintptr_t)fmu_reg[SCP_FMU_ROOT] + FMU_FIELD_ERRGSR_L(0), FWK_BIT(0));
    fwk_mmio_write_32(
        (uintptr_t)fmu_reg[SCP_FMU_1] + FMU_FIELD_ERRGSR_L(0), FWK_BIT(0));
    fwk_mmio_write_32(
        (uintptr_t)fmu_reg[SCP_FMU_1] + FMU_FIELD_ERR_STATUS(0),
        MOD_FMU_SM_SYSTEM_INPUT_ERROR << FMU_ERR_STATUS_IERR_SHIFT);

    event.id = mod_fmu_notification_id_fault;
    event.source_id = fwk_module_id_fmu;
    params = (struct mod_fmu_fault_notification_params *)event.params;
    params->critical = true;
    params->fault.device_idx = 1;
    params->fault.node_idx = 0;
    params->fault.sm_idx = MOD_FMU_SM_SYSTEM_INPUT_ERROR;
    fwk_notification_notify_ExpectAndReturn(&event, NULL, FWK_SUCCESS);
    fwk_notification_notify_IgnoreArg_count();
    TEST_ASSERT_TRUE(next_fault(true));
}

void test_fmu_set_enabled(void)
{
    int status;
    bool enabled;
    fwk_id_t id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_FMU, SCP_FMU_ROOT);

    /* Set value to true and read back */
    status = fmu_api.set_enabled(id, 2, true);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    status = fmu_api.get_enabled(id, 2, &enabled);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(true, enabled);

    /* Set value to false and read back */
    status = fmu_api.set_enabled(id, 2, false);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    status = fmu_api.get_enabled(id, 2, &enabled);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(false, enabled);
}

void test_fmu_set_count(void)
{
    int status;
    uint8_t count;
    fwk_id_t id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_FMU, SCP_FMU_ROOT);

    status = fmu_api.set_count(id, 12, 13);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    status = fmu_api.get_count(id, 12, &count);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(13, count);
}

void test_fmu_set_threshold(void)
{
    int status;
    uint8_t count;
    fwk_id_t id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_FMU, SCP_FMU_ROOT);

    status = fmu_api.set_threshold(id, 34, 25);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    status = fmu_api.get_threshold(id, 34, &count);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(25, count);
}

void test_fmu_set_upgrade_enabled(void)
{
    int status;
    bool enabled;
    fwk_id_t id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_FMU, SCP_FMU_ROOT);

    /* Set value to true and read back */
    status = fmu_api.set_upgrade_enabled(id, 101, true);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    status = fmu_api.get_upgrade_enabled(id, 101, &enabled);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(true, enabled);

    /* Set value to false and read back */
    status = fmu_api.set_upgrade_enabled(id, 101, false);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);

    status = fmu_api.get_upgrade_enabled(id, 101, &enabled);
    TEST_ASSERT_EQUAL(FWK_SUCCESS, status);
    TEST_ASSERT_EQUAL(false, enabled);
}

int fmu_test_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_fmu_init);
    RUN_TEST(test_fmu_device_init);
    RUN_TEST(test_fmu_start);
    RUN_TEST(test_fmu_process_bind_request);
    RUN_TEST(test_fmu_next_fault_simple);
    RUN_TEST(test_fmu_next_fault_upstream);
    RUN_TEST(test_fmu_set_enabled);
    RUN_TEST(test_fmu_set_count);
    RUN_TEST(test_fmu_set_threshold);
    RUN_TEST(test_fmu_set_upgrade_enabled);

    return UNITY_END();
}

#if !defined(TEST_ON_TARGET)
int main(void)
{
    return fmu_test_main();
}
#endif
