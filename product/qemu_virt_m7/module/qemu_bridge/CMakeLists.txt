#
# Arm SCP/MCP Software
# Copyright (c) 2026, Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_library(${SCP_MODULE_TARGET} STATIC)
target_include_directories(
    ${SCP_MODULE_TARGET}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_SOURCE_DIR}/module/transport/include")
target_sources(
    ${SCP_MODULE_TARGET}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/mod_qemu_bridge.c")
