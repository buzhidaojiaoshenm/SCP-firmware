#
# Arm SCP/MCP Software
# Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_executable(rdaspen-si0-bl2)

target_include_directories(rdaspen-si0-bl2 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(rdaspen-si0-bl2 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/config_armv8r_mpu.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/config_gicx00.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/config_pl011.c"
    )
