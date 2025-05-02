/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Definitions for timer module configuration data in SCP firmware.
 */

#ifndef SI0_CFGD_TIMER_H
#define SI0_CFGD_TIMER_H

#define SI0_ALARM_ELEMENT_IDX 0

/* Sub-element indexes (alarms) for SI0 timer device */
enum si0_cfgd_mod_timer_alarm_idx {
#ifdef BUILD_HAS_SCMI_NOTIFICATIONS
    SI0_CFGD_SCMI_NOTIFICATION_ALARM_IDX,
#endif
    SI0_CFGD_MOD_TIMER_ALARM_IDX_COUNT,
};

/* Sub-element indexes (alarms) for SI0 timer device */
enum si0_cfgd_mod_timer_si0_timer_alarm_idx {
    SI0_CFGD_DEBUGGER_CLI_IDX,
    SI0_CFGD_MOD_TIMER_SI0_TIMER_ALARM_IDX_COUNT,
};

#endif /* SI0_CFGD_TIMER_H */
