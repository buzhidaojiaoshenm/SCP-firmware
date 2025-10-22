/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Telemetry and module contexts.
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <mod_telemetry.h>

/*!
 * \brief Global telemetry module context.
 *
 * Stores state, telemetry sources, and SHMTI management structures.
 */
struct mod_telemetry_context {
    /*! Flag indicating telemetry is enabled */
    bool telemetry_enabled;
    /*! Telemetry sampling rate */
    uint32_t current_sampling_rate_msecs;
    /*! Total number of Data Events */
    uint32_t total_de_count;
    /*! Total enabled Data Events */
    uint32_t total_de_enabled_count;
    /*! Telemetry configuration */
    const struct mod_telemetry_config *config;
    /*! Number of SHMTIs. */
    uint32_t shmti_count;
};

#endif /* !TELEMETRY_H */
