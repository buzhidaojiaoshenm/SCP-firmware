/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_timer.h>

#include <fwk_element.h>
#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

#include <fmw_cmsis.h>

enum config_timer_element_idx {
    CONFIG_TIMER_ELEMENT_IDX_REFCLK,
    CONFIG_TIMER_ELEMENT_IDX_COUNT,
};

enum config_timer_refclk_alarm_idx {
    CONFIG_TIMER_REFCLK_ALARM_IDX0,
    CONFIG_TIMER_REFCLK_ALARM_IDX_COUNT,
};

static const struct fwk_element timer_element_table[] = {
    [CONFIG_TIMER_ELEMENT_IDX_REFCLK] = {
        .name = "REFCLK",
        .data = &(struct mod_timer_dev_config) {
            .id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_QEMU_TIMER, 0),
            .timer_irq = TIMER0_IRQn,
        },
        .sub_element_count = CONFIG_TIMER_REFCLK_ALARM_IDX_COUNT,
    },
    [CONFIG_TIMER_ELEMENT_IDX_COUNT] = { 0 },
};

const struct fwk_module_config config_timer = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(timer_element_table),
};
