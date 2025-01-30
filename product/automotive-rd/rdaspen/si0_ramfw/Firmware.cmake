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

set(SCP_ENABLE_NOTIFICATIONS_INIT TRUE)

set(SCP_ENABLE_OUTBAND_MSG_SUPPORT TRUE)

set(SCP_ENABLE_AE_EXTENSION TRUE)

set(SCP_ENABLE_SCMI_NOTIFICATIONS TRUE)

list(PREPEND SCP_MODULE_PATHS
     "${CMAKE_CURRENT_LIST_DIR}/../module/si0_platform"
     "${CMAKE_CURRENT_LIST_DIR}/../module/ros_clock")

list(APPEND SCP_MODULES
    "armv8r-mpu"
    "pl011"
    "gicx00"
    "system-pll"
    "ros-clock"
    "clock"
    "gtimer"
    "timer"
    "sid"
    "system-info"
    "pcid"
    "mhu3"
    "transport"
    "ppu-v1"
    "power-domain"
    "cmn-cyprus"
    "apcontext"
    "scmi"
    "sds"
    "scmi-power-domain"
    "scmi-system-power"
    "si0-platform"
)
