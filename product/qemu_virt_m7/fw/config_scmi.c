/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_scmi.h>

#include <fwk_module.h>

static const struct mod_scmi_agent agent_table[] = {
    [1] = {
        .type = SCMI_AGENT_TYPE_OSPM,
        .name = "OSPM",
    },
};

struct fwk_module_config config_scmi = {
    .data = &(struct mod_scmi_config) {
        .protocol_count_max = 8,
        .agent_count = 1,
        .agent_table = agent_table,
        .vendor_identifier = "qemu",
        .sub_vendor_identifier = "virt",
    },
};
