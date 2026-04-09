/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FMW_LOG_H
#define FMW_LOG_H

/*
 * Disable framework log buffering for qemu_virt_m7 so dense CMN discovery
 * logs are emitted directly instead of being dropped and summarized later.
 */
#define FMW_LOG_BUFFER_SIZE 0

#endif /* FMW_LOG_H */
