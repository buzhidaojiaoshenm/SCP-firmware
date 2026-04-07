/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <fwk_module.h>

/*
 * Transport is enabled as the SCMI channel container. Channels are added when
 * the QEMU mailbox bridge and shared-memory layout are fixed.
 */
const struct fwk_module_config config_transport = { 0 };
