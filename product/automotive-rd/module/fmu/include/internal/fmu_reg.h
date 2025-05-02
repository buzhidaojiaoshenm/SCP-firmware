/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MOD_FMU_REG_H
#define MOD_FMU_REG_H

#include <fwk_macros.h>
#include <fwk_mmio.h>

#include <stdint.h>

#define FMU_FIELD_ERR_FR(n)     (0x0000 + (64 * (n)))
#define FMU_FIELD_ERR_CTRL(n)   (0x0008 + (64 * (n)))
#define FMU_FIELD_ERR_STATUS(n) (0x0010 + (64 * (n)))
#define FMU_FIELD_ERRIMPDEF(n)  (0x8000 + (8 * (n)))
#define FMU_FIELD_SYS_KEY       0x8BFC
#define FMU_FIELD_ERRGSR_L(n)   (0xE000 + (8 * (n)))
#define FMU_FIELD_ERRGSR_H(n)   (0xE004 + (8 * n))

#define FMU_ERR_FR_ED_MASK FWK_GEN_MASK(1, 0)

#define FMU_ERR_STATUS_IERR_MASK               FWK_GEN_MASK(12, 8)
#define FMU_ERR_STATUS_IERR_SHIFT              8
#define FMU_ERR_STATUS_IERR_APB_SW_MASK        FWK_BIT(8)
#define FMU_ERR_STATUS_IERR_INC_SEQ_MASK       FWK_BIT(9)
#define FMU_ERR_STATUS_IERR_APB_PARITY_MASK    FWK_BIT(10)
#define FMU_ERR_STATUS_IERR_ERR_IN_PARITY_MASK FWK_BIT(11)
#define FMU_ERR_STATUS_IERR_ERR_IN_MASK        FWK_BIT(12)

#define FMU_ERR_CTRL_ED_MASK  FWK_BIT(0)
#define FMU_ERR_CTRL_FI_MASK  FWK_BIT(3)
#define FMU_ERR_CTRL_UE_MASK  FWK_BIT(4)
#define FMU_ERR_CTRL_CFI_MASK FWK_BIT(8)
#define FMU_ERR_CTRL_CI_MASK  FWK_BIT(13)
#define FMU_ERR_CTRL_ENABLE_MASK \
    (FMU_ERR_CTRL_ED_MASK | FMU_ERR_CTRL_FI_MASK | FMU_ERR_CTRL_UE_MASK | \
     FMU_ERR_CTRL_CFI_MASK | FMU_ERR_CTRL_CI_MASK)

#define FMU_ERRIMPDEF_UE_MASK   FWK_BIT(0)
#define FMU_ERRIMPDEF_IC_MASK   FWK_GEN_MASK(3, 2)
#define FMU_ERRIMPDEF_IE_MASK   FWK_GEN_MASK(13, 9)
#define FMU_ERRIMPDEF_IE_SHIFT  9
#define FMU_ERRIMPDEF_THR_MASK  FWK_GEN_MASK(31, 24)
#define FMU_ERRIMPDEF_THR_SHIFT 24
#define FMU_ERRIMPDEF_CNT_MASK  FWK_GEN_MASK(23, 16)
#define FMU_ERRIMPDEF_CNT_SHIFT 16

#define FMU_ERRGSR_MAX      5
#define FMU_ERRGSR_NUM_BITS 32

#define FMU_SYS_KEY_UNLOCK 0xBE

static inline uint32_t fmu_read_32(uintptr_t base, uintptr_t offset)
{
    return fwk_mmio_read_32(base + offset);
}

static inline void fmu_write_32(
    uintptr_t base,
    uintptr_t offset,
    uint32_t value)
{
    fwk_mmio_write_32(base + FMU_FIELD_SYS_KEY, FMU_SYS_KEY_UNLOCK);
    fwk_mmio_write_32(base + offset, value);
}

#endif
