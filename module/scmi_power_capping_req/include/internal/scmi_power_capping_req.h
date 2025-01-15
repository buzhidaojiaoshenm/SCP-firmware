/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *      SCMI Power Capping Requestor protocol support.
 */

#ifndef INTERNAL_SCMI_POWER_CAPPING_REQ_H
#define INTERNAL_SCMI_POWER_CAPPING_REQ_H

/*
 * PROTOCOL_SET_CAP_ATTRIBUTES
 */

struct scmi_pcap_req_set_cap_a2p {
    uint32_t flags;
    uint32_t power_cap;
};

#endif /* INTERNAL_SCMI_POWER_CAPPING_REQ */
