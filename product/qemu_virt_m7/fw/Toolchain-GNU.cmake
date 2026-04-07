#
# Arm SCP/MCP Software
# Copyright (c) 2026, Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

include_guard()

set(CMAKE_SYSTEM_PROCESSOR "cortex-m7")

get_filename_component(
    QEMU_VIRT_SOC_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(QEMU_VIRT_M7_GNU_TOOLCHAIN_DIR
    "${QEMU_VIRT_SOC_ROOT}/toolchains/arm-none-eabi-gcc"
    CACHE PATH "Path to the project-local GNU Arm bare-metal toolchain")

if(NOT EXISTS "${QEMU_VIRT_M7_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc")
    message(FATAL_ERROR
        "Project-local arm-none-eabi-gcc was not found. "
        "Expected: ${QEMU_VIRT_M7_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc")
endif()

set(CMAKE_TOOLCHAIN_PREFIX
    "${QEMU_VIRT_M7_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-")

set(CMAKE_ASM_COMPILER_TARGET "arm-none-eabi")
set(CMAKE_C_COMPILER_TARGET "arm-none-eabi")
set(CMAKE_CXX_COMPILER_TARGET "arm-none-eabi")

set(CMAKE_TOP_DIR "${CMAKE_CURRENT_LIST_DIR}/../../..")
include("${CMAKE_TOP_DIR}/cmake/Toolchain/GNU-Baremetal.cmake")
