#
# Arm SCP/MCP Software
# Copyright (c) 2026, Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(SCP_FIRMWARE "qemu_virt_m7")
set(SCP_FIRMWARE_TARGET "qemu_virt_m7")

set(SCP_TOOLCHAIN_INIT "GNU")
set(SCP_GENERATE_FLAT_BINARY_INIT TRUE)

set(SCP_ARCHITECTURE "arm-m")
set(SCP_ENABLE_NEWLIB_NANO FALSE)
set(SCP_ENABLE_IPO_INIT FALSE)
set(SCP_ENABLE_NOTIFICATIONS TRUE)
set(SCP_ENABLE_OUTBAND_MSG_SUPPORT TRUE)

list(PREPEND SCP_MODULE_PATHS "${CMAKE_CURRENT_LIST_DIR}/../module/qemu_bridge")
list(PREPEND SCP_MODULE_PATHS "${CMAKE_CURRENT_LIST_DIR}/../module/qemu_sysinfo")
list(APPEND SCP_MODULES "pl011")
list(APPEND SCP_MODULES "clock")
list(APPEND SCP_MODULES "gtimer")
list(APPEND SCP_MODULES "sp805")
list(APPEND SCP_MODULES "qemu-sysinfo")
list(APPEND SCP_MODULES "system-info")
list(APPEND SCP_MODULES "qemu-bridge")
list(APPEND SCP_MODULES "timer")
list(APPEND SCP_MODULES "transport")
list(APPEND SCP_MODULES "scmi")
list(APPEND SCP_MODULES "cmn-cyprus")
