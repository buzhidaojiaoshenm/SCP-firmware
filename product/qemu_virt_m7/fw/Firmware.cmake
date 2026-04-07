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

list(APPEND SCP_MODULES "stdio")
