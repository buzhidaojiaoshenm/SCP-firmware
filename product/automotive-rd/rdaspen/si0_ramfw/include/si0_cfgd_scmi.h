/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Definitions for SCMI module configuration data in SCP firmware.
 */

#ifndef SI0_CFGD_SCMI_H
#define SI0_CFGD_SCMI_H

/* SCMI agent identifier indexes in the SCMI agent table */
enum si0_scmi_agent_idx {
    /* 0 is reserved for the platform */
    SI0_SCMI_AGENT_IDX_RSE = 1,
    SI0_SCMI_AGENT_IDX_PSCI = 2,
    SI0_SCMI_AGENT_IDX_COUNT,
};

/* Module 'scmi' element indexes (SCMI services supported) */
enum si0_cfgd_mod_scmi_element_idx {
    SI0_CFGD_MOD_SCMI_EIDX_RSE,
    SI0_CFGD_MOD_SCMI_EIDX_PSCI,
#ifdef BUILD_HAS_SCMI_NOTIFICATIONS
    SI0_CFGD_MOD_SCMI_EIDX_RSE_P2A, /* SCP to RSE notification */
#endif
    SI0_CFGD_MOD_SCMI_EIDX_COUNT,
};

#endif /* SI0_CFGD_SCMI_H */
