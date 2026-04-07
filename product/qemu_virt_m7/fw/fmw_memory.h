/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FMW_MEMORY_H
#define FMW_MEMORY_H

#define FMW_MEM_MODE ARCH_MEM_MODE_SINGLE_REGION

/*
 * QEMU mps2-an500 maps ZBT SSRAM1 at 0x00000000..0x003fffff.
 */
#define FMW_MEM0_BASE 0x00000000
#define FMW_MEM0_SIZE 0x00400000

#endif /* FMW_MEMORY_H */

