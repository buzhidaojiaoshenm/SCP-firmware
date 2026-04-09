/*
 * Arm SCP/MCP Software
 * Copyright (c) 2026, Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mod_cmn_cyprus.h>

#include <fwk_id.h>
#include <fwk_macros.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

#include <stdint.h>

#define QEMU_VIRT_M7_CMN_BASE       UINT32_C(0x60000000)
#define QEMU_VIRT_M7_CMN_MESH_X     3U
#define QEMU_VIRT_M7_CMN_MESH_Y     3U

/*
 * Node IDs follow the 3x3 mesh in the user-provided Cyprus topology image.
 * The CMN model only needs the discovery-visible nodes and stable target IDs
 * for HN-S SAM / RNSAM programming.
 */
#define NODE_ID_HND                 2U
#define NODE_ID_HNI0                20U

#define NODE_ID_SBSX0               0U
#define NODE_ID_SBSX1               16U
#define NODE_ID_SBSX2               18U
#define NODE_ID_SBSX3               19U
#define NODE_ID_SBSX4               50U
#define NODE_ID_SBSX5               51U
#define NODE_ID_SBSX6               82U
#define NODE_ID_SBSX7               83U

static const unsigned int snf_table[] = {
    NODE_ID_SBSX0,
    NODE_ID_SBSX1,
    NODE_ID_SBSX2,
    NODE_ID_SBSX3,
    NODE_ID_SBSX4,
    NODE_ID_SBSX5,
    NODE_ID_SBSX6,
    NODE_ID_SBSX7,
};

static const struct mod_cmn_cyprus_mem_region_map mmap[] = {
    {
        .base = UINT64_C(0x000000000000),
        .size = UINT64_C(256) * FWK_TIB,
        .type = MOD_CMN_CYPRUS_MEM_REGION_TYPE_SYSCACHE,
        .hns_pos_start = { 0, 0, 1, 0 },
        .hns_pos_end = { 2, 1, 2, 0 },
    },
    {
        .base = UINT64_C(0x000000000000),
        .size = UINT64_C(128) * FWK_MIB,
        .type = MOD_CMN_CYPRUS_MEM_REGION_TYPE_SYSCACHE_SUB,
        .node_id = NODE_ID_SBSX2,
    },
    {
        .base = UINT64_C(0x0010000000),
        .size = UINT64_C(256) * FWK_MIB,
        .type = MOD_CMN_CYPRUS_MEM_REGION_TYPE_IO,
        .node_id = NODE_ID_HND,
    },
    {
        .base = UINT64_C(0x0100000000),
        .size = UINT64_C(1) * FWK_GIB,
        .type = MOD_CMN_CYPRUS_MEM_REGION_TYPE_IO,
        .node_id = NODE_ID_HND,
    },
    {
        .base = UINT64_C(0x0140000000),
        .size = UINT64_C(1) * FWK_GIB,
        .type = MOD_CMN_CYPRUS_MEM_REGION_TYPE_IO,
        .node_id = NODE_ID_HNI0,
    },
};

static const struct mod_cmn_cyprus_config cmn_config_table[] = {
    [0] = {
        .periphbase = QEMU_VIRT_M7_CMN_BASE,
        .mesh_size_x = QEMU_VIRT_M7_CMN_MESH_X,
        .mesh_size_y = QEMU_VIRT_M7_CMN_MESH_Y,
        .mmap_table = mmap,
        .mmap_count = FWK_ARRAY_SIZE(mmap),
        .hnf_sam_config = {
            .snf_table = snf_table,
            .snf_count = FWK_ARRAY_SIZE(snf_table),
            .hnf_sam_mode = MOD_CMN_CYPRUS_HNF_SAM_MODE_DIRECT_MAPPING,
        },
        .hns_cal_mode = false,
        .rnsam_scg_config = {
            .scg_hashing_mode =
                MOD_CMN_CYPRUS_RNSAM_SCG_POWER_OF_TWO_HASHING,
        },
    },
};

static const struct mod_cmn_cyprus_config_table cmn_config_data = {
    .chip_config_data = cmn_config_table,
    .chip_count = 1U,
    .timer_id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_TIMER, 0),
};

const struct fwk_module_config config_cmn_cyprus = {
    .data = (void *)&cmn_config_data,
};
