/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_qemu_sysinfo.h>

#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_status.h>

static struct mod_system_info qemu_system_info = {
    .product_id = 0,
    .config_id = 0,
    .multi_chip_mode = false,
    .chip_id = 0,
    .name = "qemu_virt_m7",
};

static struct mod_system_info *qemu_get_driver_data(void)
{
    return &qemu_system_info;
}

static const struct mod_system_info_get_driver_data_api qemu_sysinfo_api = {
    .get_driver_data = qemu_get_driver_data,
};

static int qemu_sysinfo_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    (void)module_id;
    (void)data;

    return (element_count == 0) ? FWK_SUCCESS : FWK_E_DATA;
}

static int qemu_sysinfo_process_bind_request(
    fwk_id_t requester_id,
    fwk_id_t id,
    fwk_id_t api_id,
    const void **api)
{
    (void)requester_id;
    (void)id;

    if (!fwk_id_is_equal(api_id, FWK_ID_API(FWK_MODULE_IDX_QEMU_SYSINFO, 0))) {
        return FWK_E_PARAM;
    }

    *api = &qemu_sysinfo_api;
    return FWK_SUCCESS;
}

const struct fwk_module module_qemu_sysinfo = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = qemu_sysinfo_init,
    .api_count = 1,
    .process_bind_request = qemu_sysinfo_process_bind_request,
};
