#
# Arm SCP/MCP Software
# Copyright (c) 2026, Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_library(module-qemu-sysinfo)

target_include_directories(
    module-qemu-sysinfo
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
           "${CMAKE_SOURCE_DIR}/module/system_info/include")

target_sources(
    module-qemu-sysinfo
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/mod_qemu_sysinfo.c")

target_link_libraries(module-qemu-sysinfo PUBLIC framework)
