/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "si0_irq.h"
#include "si0_mmap.h"

#include <mod_fmu.h>

#include <fwk_log.h>
#include <fwk_module.h>

enum fmu_device {
    SI0_FMU_ROOT,
    SI0_FMU_1,
    SI0_FMU_2,
    SI0_FMU_3,
    SI0_FMU_4,
    SI0_FMU_COUNT,
};

static const struct fwk_element fmu_devices[SI0_FMU_COUNT + 1] = {
    [SI0_FMU_ROOT] = {
        .name = "fmu0",
        .data = &((struct mod_fmu_dev_config) {
            .base = SI0_FMU0_BASE,
            .parent = MOD_FMU_PARENT_NONE,
        }),
    },
    [SI0_FMU_1] = {
        .name = "fmu1",
        .data = &((struct mod_fmu_dev_config) {
            .base = SI0_FMU1_BASE,
            .parent = SI0_FMU_ROOT,
            .parent_cr_index = 0,
            .parent_ncr_index = 1,
        }),
    },
    [SI0_FMU_2] = {
        .name = "fmu2",
        .data = &((struct mod_fmu_dev_config) {
            .base = SI0_FMU2_BASE,
            .parent = SI0_FMU_ROOT,
            .parent_cr_index = 2,
            .parent_ncr_index = 3,
        }),
    },
    [SI0_FMU_3] = {
        .name = "fmu3",
        .data = &((struct mod_fmu_dev_config) {
            .base = SI0_FMU3_BASE,
            .parent = SI0_FMU_ROOT,
            .parent_cr_index = 4,
            .parent_ncr_index = 5,
        }),
    },
    [SI0_FMU_4] = {
        .name = "fmu4",
        .data = &((struct mod_fmu_dev_config) {
            .base = SI0_FMU4_BASE,
            .parent = SI0_FMU_ROOT,
            .parent_cr_index = 6,
            .parent_ncr_index = 7,
        }),
    },
    [SI0_FMU_COUNT] = {0},
};

/*
 * Configuration for the debugger CLI module
 */
struct fwk_module_config config_fmu = {
    .data =
        &(struct mod_fmu_config){
            .irq_critical = CL0_FMU_CRITICAL,
            .irq_non_critical = CL0_FMU_NON_CRITICAL,
        },
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(fmu_devices),
};
