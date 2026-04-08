/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FMW_MEMORY_H
#define FMW_MEMORY_H

#define FMW_MEM_MODE ARCH_MEM_MODE_DUAL_REGION_NO_RELOCATION

/* SCP boot ITCM */
#define FMW_MEM0_BASE 0x00000000
#define FMW_MEM0_SIZE 0x000c0000

/* SCP DTCM */
#define FMW_MEM1_BASE 0x20000000
#define FMW_MEM1_SIZE 0x00080000

#endif /* FMW_MEMORY_H */
