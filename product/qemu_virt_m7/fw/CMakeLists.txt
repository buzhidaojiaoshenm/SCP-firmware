#
# Arm SCP/MCP Software
# Copyright (c) 2026, Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_executable(qemu_virt_m7)

target_include_directories(
    qemu_virt_m7
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
           "${CMAKE_SOURCE_DIR}/arch/arm/arm-m/include"
           "${CMAKE_SOURCE_DIR}/arch/arm/common/include")

target_sources(qemu_virt_m7
               PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/config_stdio.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_system.c")

target_link_libraries(qemu_virt_m7 PUBLIC cmsis::core-m)

target_include_directories(
    qemu_virt_m7
    PUBLIC $<TARGET_PROPERTY:cmsis::core-m,INTERFACE_INCLUDE_DIRECTORIES>)
