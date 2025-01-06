/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     SCP Platform Support
 */

#ifndef MOD_SI0_PLATFORM_H
#define MOD_SI0_PLATFORM_H

#include <fwk_id.h>

/*!
 * \addtogroup GroupPLATFORMModule PLATFORM Product Modules
 * @{
 */

/*!
 * \defgroup GroupSI0Platform SI0 Platform Support
 * @{
 */

/*!
 * \brief Indices of the interfaces exposed by the module.
 */
enum mod_si0_platform_api_idx {
    /*! Interface for Transport module */
    MOD_SI0_PLATFORM_API_IDX_TRANSPORT_SIGNAL,
    /*! Number of exposed interfaces */
    MOD_SI0_PLATFORM_API_COUNT
};

/*!
 * \brief SCP platform configuration data.
 */
struct mod_si0_platform_config {
    /*! Transport channel identifier */
    fwk_id_t transport_id;
};

/*!
 * @}
 */

/*!
 * @}
 */

#endif /* MOD_SI0_PLATFORM_H */
