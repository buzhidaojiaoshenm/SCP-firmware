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
               PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/config_pl011.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_clock.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_gtimer.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_sp805.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_qemu_sysinfo.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_system_info.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_timer.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_transport.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_cmn_cyprus.c"
                       "${CMAKE_CURRENT_SOURCE_DIR}/config_system.c")

target_link_libraries(qemu_virt_m7 PUBLIC cmsis::core-m)

target_include_directories(
    qemu_virt_m7
    PUBLIC $<TARGET_PROPERTY:cmsis::core-m,INTERFACE_INCLUDE_DIRECTORIES>)
