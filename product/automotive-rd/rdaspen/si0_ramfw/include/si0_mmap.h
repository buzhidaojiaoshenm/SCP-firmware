/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SI0_MMAP_H
#define SI0_MMAP_H

#include <fwk_macros.h>

#define SI0_SRAM_BASE    0x120000000
#define SI0_ITC_RAM_SIZE (256 * 1024)
#define SI0_DTC_RAM_SIZE (256 * 1024)
#define SI0_ITC_RAM_BASE SI0_SRAM_BASE
#define SI0_DTC_RAM_BASE (SI0_SRAM_BASE + SI0_ITC_RAM_SIZE)

#define SI0_GIC_BASE  0x30000000
#define SI0_GIC_SIZE  (1 * FWK_MIB)
#define SI0_GICD_BASE 0x30000000
#define SI0_GICR_BASE 0x30040000

#define SI0_PERIPHERAL_BASE 0x2A000000
#define SI0_PERIPHERAL_SIZE (16 * FWK_MIB)
#define SI0_UART_BASE       0x2A400000

#endif /* SI0_MMAP_H */
