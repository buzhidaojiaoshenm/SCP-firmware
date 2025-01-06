#
# Arm SCP/MCP Software
# Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(SCP_FIRMWARE "rdaspen-si0-bl2")
set(SCP_FIRMWARE_TARGET "rdaspen-si0-bl2")

set(SCP_TOOLCHAIN_INIT "GNU")

set(SCP_GENERATE_FLAT_BINARY_INIT TRUE)

set(SCP_ARCHITECTURE "aarch64")

list(PREPEND SCP_MODULE_PATHS
     "${CMAKE_CURRENT_LIST_DIR}/../module/si0_platform")

list(APPEND SCP_MODULES
    "armv8r-mpu"
    "pl011"
    "gicx00"
    "si0-platform"
)
