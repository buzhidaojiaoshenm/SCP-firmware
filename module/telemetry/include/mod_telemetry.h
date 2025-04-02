/*
 * Arm SCP/MCP Software
 * Copyright (c) 2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Description:
 *     Telemetry HAL module (See SCMI Specification > 3.2).
 */

#ifndef MOD_TELEMETRY_H
#define MOD_TELEMETRY_H

#include <fwk_id.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*!
 * \addtogroup GroupModules
 * \{
 */

/*!
 * \defgroup GroupTelemetry Telemetry Support
 * \{
 */

/*!
 * \brief Defines the maximum telemetry sources and data event limits.
 */
#define MOD_TELEMETRY_MAX_TELEMETRY_SOURCES 16

/*! Maximum Data Events per telemetry source */
#define MOD_TELEMETRY_MAX_DE_PER_SOURCE     256

/*!
 * \brief Defines various field types as specified for TDCF in the
 *        SCMI specification > 3.2.
 */

/*! Size in bytes for Header MetadataH + Header MetadataL */
#define MOD_TELEMETRY_HEADER_METADATA_N_BYTES 8

/*! Size in bytes for StartMatchSequenceH + StartMatchSequenceL */
#define MOD_TELEMETRY_HEADER_MATCH_SEQ_N_BYTES 8

/*! Size in bytes for Line-TimestampH + Line-TimestampL */
#define MOD_TELEMETRY_TS_N_BYTES 8

/*! Size in bytes for Line-DataH + Line-DataL */
#define MOD_TELEMETRY_DE_DATA_N_BYTES 8

/*! Size in bytes for Line-ID */
#define MOD_TELEMETRY_DE_ID_N_BYTES 4

/*! Size in bytes for Line-Metadata */
#define MOD_TELEMETRY_LINE_META_DATA_SIZE_BYTES 4

/*! Total size in bytes for Line that does not include a timestamp */
#define MOD_TELEMETRY_DE_SIZE_NON_TS \
    (MOD_TELEMETRY_DE_DATA_N_BYTES + MOD_TELEMETRY_DE_ID_N_BYTES + \
     MOD_TELEMETRY_LINE_META_DATA_SIZE_BYTES)

/*! Total size in bytes for Line that includes a timestamp */
#define MOD_TELEMETRY_DE_SIZE_TS \
    (MOD_TELEMETRY_DE_SIZE_NON_TS + MOD_TELEMETRY_TS_N_BYTES)

/*! Size in bytes for a block timestamp line */
#define MOD_TELEMETRY_DE_BLOCK_LINE_N_BYTES \
    (MOD_TELEMETRY_TS_N_BYTES + MOD_TELEMETRY_DE_ID_N_BYTES + \
     MOD_TELEMETRY_LINE_META_DATA_SIZE_BYTES)

/*!
 * \brief SHMTI (Shared-memory based telemetry interfaces) descriptor.
 */
struct mod_telemetry_shmti_desc {
    /*! SHMTI ID. */
    uint32_t shmti_id;
    /*! Start address of the SHMTI region (low part). */
    uint32_t addr_lo;
    /*! Start address of the SHMTI region (high part). */
    uint32_t addr_hi;
    /*! Length of the SHMTI region. */
    size_t length;
};

/*!
 * \brief Holds SHMTI info such as addresses and size.
 */
struct mod_telemetry_shmti_info {
    /*! Start address of the SHMTI region. */
    uintptr_t start_addr;
    /*! Agent (e.g. AP) view of the start address of the SHMTI region. */
    uint64_t start_ap_addr;
    /*! Length of the SHMTI region. */
    size_t length;
};

/*!
 * \brief DE(Data Event) descriptor.
 */
struct mod_telemetry_de_desc {
    /*! DE ID. */
    uint32_t de_id;
    /*! Attribute 1, Refer SCMI specification for details  */
    uint32_t attributes_1;
    /*! Attribute 2. Refer SCMI specification for details */
    uint32_t attributes_2;
    /*! Attribute 3, Reserved must be zero */
    uint32_t attributes_3;
    /*! Attribute 4, Lower 32 bits of the FastChannel address. */
    uint32_t attributes_4;
    /*! Attribute 5, Higher 32 bits of the FastChannel address. */
    uint32_t attributes_5;
    /*! Attribute 6, Size of the FastChannel is bytes. */
    uint32_t attributes_6;
};

/*!
 * \brief Various DE(Data Event) types as per the SCMI specification.
 */
enum mod_telemetry_de_types {
    mod_telemetry_de_type_unspecified = 0x0,
    mod_telemetry_de_type_accumulating_residency = 0x1,
    mod_telemetry_de_type_accumulating_count = 0x2,
    mod_telemetry_de_type_accumulating_other = 0x3,
    mod_telemetry_de_type_instantaneous = 0x4,
    mod_telemetry_de_type_instantaneous_other = 0x5,
    mod_telemetry_de_type_averaging = 0x6,
    mod_telemetry_de_type_status = 0x7,
};

/*!
 * \brief Various DE Component types as per the SCMI specification.
 */
enum mod_telemetry_de_comp_type_id {
    mod_telemetry_de_component_unspecified = 0x0,
    mod_telemetry_de_cpu = 0x1,
    mod_telemetry_de_cluster = 0x2,
    mod_telemetry_de_gpu = 0x3,
    mod_telemetry_de_npu = 0x4,
    mod_telemetry_de_interconnect = 0x5,
    mod_telemetry_de_memory_controller = 0x6,
    mod_telemetry_de_l1_cache = 0x7,
    mod_telemetry_de_l2_cache = 0x8,
    mod_telemetry_de_l3_cache = 0x9,
    mod_telemetry_de_last_level_cache = 0xA,
    mod_telemetry_de_system_cache = 0xB,
    mod_telemetry_de_display_controller = 0xC,
    mod_telemetry_de_ipu_camera = 0xD,
    mod_telemetry_de_chiplet = 0xE,
    mod_telemetry_de_package = 0xF,
    mod_telemetry_de_soc = 0x10,
    mod_telemetry_de_system = 0x11,
    mod_telemetry_de_smcu = 0x12,
    mod_telemetry_de_accelerator = 0x13,
    mod_telemetry_de_battery = 0x14,
    mod_telemetry_de_battery_charger = 0x15,
    mod_telemetry_de_pmic = 0x16,
    mod_telemetry_de_board = 0x17,
    mod_telemetry_de_memory = 0x18,
    mod_telemetry_de_device = 0x19,
    mod_telemetry_de_subcomponent = 0x1A,
    mod_telemetry_de_lid = 0x1B,
    mod_telemetry_de_display = 0x1C,
};

/*!
 * \brief Architected DE IDs specified by the SCMI specification.
 */
enum mod_telemetry_de_id {
    mod_telemetry_de_id_soc_energy = 0xA000,
    mod_telemetry_de_id_total_cpu_energy = 0xA001,
    mod_telemetry_de_id_dram_energy = 0xA002,
    mod_telemetry_de_id_gpu_energy = 0xA003,
    mod_telemetry_de_id_soc_temp = 0xA004,
    mod_telemetry_de_id_max_cpu_temp = 0xA005,
    mod_telemetry_de_id_gpu_temp = 0xA006,
    mod_telemetry_de_id_dram_temp = 0xA007,
    mod_telemetry_de_id_interconnect_freq = 0xA008,
    mod_telemetry_de_id_gpu_freq = 0xA009,
    mod_telemetry_de_id_memory_bw = 0xA00A,
    mod_telemetry_de_id_dram_speed = 0xA00B,
    mod_telemetry_de_id_num_cpu_throttle_events = 0xA00C,
    mod_telemetry_de_id_num_gpu_throttle_events = 0xA00D,
    mod_telemetry_de_id_num_dram_throttle_events = 0xA00E,
    mod_telemetry_de_id_num_soc_throttle_events = 0xA00F,
    mod_telemetry_de_id_cpu_active_throttle = 0xA010,
    mod_telemetry_de_id_gpu_active_throttle = 0xA011,
    mod_telemetry_de_id_dram_active_throttle = 0xA012,
    mod_telemetry_de_id_soc_active_throttle = 0xA013,
};

/*!
 * \brief Interval Format types.
 *
 * SCMI Specification specifies two types of update intervals for telemetry
 * data to be refreshed/updated. These intervals could be discrete or linear.
 */
enum mod_telemetry_update_interval_formats {
    MOD_TELEMETRY_UPDATE_INTERVALS_DISCRETE,
    MOD_TELEMETRY_UPDATE_INTERVALS_LINEAR,
};

/*!
 * \brief The exponent length within a word.
 */
struct mod_telemetry_update_interval_exponent {
    /*! Exponent field width */
    int32_t value : 5;
};

/*! Number of bits for exponent field in a 32 bit interval value */
#define MOD_TELEMETRY_INTERVAL_NUM_EXPONENT_BITS (5U)
/*! Number of bits for second field in a 32 bit interval value */
#define MOD_TELEMETRY_INTERVAL_NUM_SECONDS_BITS  (15U)
/*! Bit position of second field in 32 bit value */
#define MOD_TELEMETRY_INTERVAL_SECONDS_POS       (5U)

/*! Bitmask for exponent field */
#define MOD_TELEMETRY_INTERVAL_NUM_EXPONENT_MASK \
    ((1UL << MOD_TELEMETRY_INTERVAL_NUM_EXPONENT_BITS) - 1)

/*! Bitmask for second field */
#define MOD_TELEMETRY_INTERVAL_NUM_SECONDS_MASK \
    (((1UL << MOD_TELEMETRY_INTERVAL_NUM_SECONDS_BITS) - 1) \
     << MOD_TELEMETRY_INTERVAL_SECONDS_POS)

/*! Extract exponent field */
#define MOD_TELEMETRY_INTERVAL_EXPONENT(rate_value) \
    ((rate_value) & (uint32_t)MOD_TELEMETRY_INTERVAL_NUM_EXPONENT_MASK)

/*! Extract second field */
#define MOD_TELEMETRY_INTERVAL_SECONDS(rate_value) \
    (((rate_value) & (uint32_t)MOD_TELEMETRY_INTERVAL_NUM_SECONDS_MASK) >> \
     MOD_TELEMETRY_INTERVAL_SECONDS_POS)

/*!
 * \brief DE (Data Event) Status.
 *
 * This structure holds the runtime status of a Data Event (DE).
 * It includes the DE ID and its timestamp mode.
 */
struct mod_telemetry_de_status {
    /*! Unique identifier for the Data Event (DE). */
    uint32_t de_id;
    /*! Timestamp mode for this DE (2-bit field). */
    uint32_t de_ts_mode : 2;
    /*! Reserved bits for future use, must be set to 0. */
    uint32_t reserved : 30;
};

/*!
 * \brief Data Event (DE) descriptor and its runtime status.
 *
 * This structure holds a Data Event descriptor along with its status.
 */
struct mod_telemetry_de {
    /*! Descriptor defining DE attributes. */
    struct mod_telemetry_de_desc desc;
    /*! Current status and timestamp mode. */
    struct mod_telemetry_de_status status;
};

/*!
 * \brief Line types in TDCF shared memory.
 *
 * These line types define how Data Events (DEs) store timestamp information.
 * - `mod_telemetry_de_line_type_ts`: Each DE has an individual timestamp.
 * - `mod_telemetry_de_line_type_non_ts`: DEs do not have a timestamp.
 * - `mod_telemetry_de_line_type_block_ts`: Multiple DEs share a common
 * timestamp.
 */
enum mod_telemetry_de_line_type {
    /*! Each DE has an individual timestamp. */
    mod_telemetry_de_line_type_ts = 0,
    /*! DEs do not have an individual timestamp. */
    mod_telemetry_de_line_type_non_ts = 1,
    /*! Multiple DEs share a common timestamp block. */
    mod_telemetry_de_line_type_block_ts = 2
};

/*!
 * \brief DE Pool structure.
 *
 * This structure defines a pool of Data Events allocated in shared memory.
 */
struct mod_telemetry_de_pool {
    /*! ID of the Shared Memory Telemetry Interface (SHMTI). */
    uint32_t shmti_id;
    /*! Base address of the DE pool in shared memory. */
    uintptr_t addr;
    /*! Type of DE line (timestamped or not). */
    enum mod_telemetry_de_line_type de_type;
    /*! Size of each DE in bytes. */
    uint8_t de_size;
    /*! Total number of DEs allocated in this pool. */
    uint8_t num_de;
};

/*!
 * \brief Telemetry Configuration
 *
 * This structure defines the telemetry configuration settings, including
 * update intervals, shared memory details, and miscellaneous settings.
 */
struct mod_telemetry_config {
    /* --- Sampling Rates Configuration --- */

    /*! Number of telemetry update intervals available. */
    uint32_t num_intervals;

    /*! Format of the update intervals (discrete or linear). */
    enum mod_telemetry_update_interval_formats interval_format;

    /*!
     * Pointer to an array of sampling rates in Hz.
     *
     * \note The array should contain `num_intervals` elements.
     */
    uint32_t *sampling_rates;

    /*! Alarm ID used for scheduling periodic telemetry updates. */
    fwk_id_t alarm_id;

    /* --- SHMTI Configuration --- */

    /*! Number of SHMTI (Shared Memory Telemetry Interface) regions. */
    uint32_t shmti_count;

    /*! Pointer to an array of SHMTI region info.
     *
     * \note The array size should be `shmti_count`.
     */
    struct mod_telemetry_shmti_info *shmti_list;

    /* --- Miscellaneous Configuration --- */

    /*! Enables asynchronous telemetry data updates.
     *
     * \note When enabled, telemetry updates do not block and run independently.
     */
    bool async_support;

    /*! Enables continuous update notifications for telemetry data.
     *
     * \note When enabled, notifications are sent upon every telemetry update.
     */
    bool continuous_update_notif_support;
};

/*!
 * \brief SHMTI API.
 */
struct mod_telemetry_shmti_api {
    /*!
     * \brief Allocate a pool of Data Events (DEs) with individual timestamps.
     *
     * \param[in] num_de Number of Data Events.
     * \param[in,out] de_pool Pointer to the DE pool structure to be populated.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*allocate_ts_de_pool_shmti)(
        uint32_t num_de,
        struct mod_telemetry_de_pool *de_pool);
    /*!
     * \brief Allocate a pool of Data Events (DEs) without individual
     *    timestamps.
     *
     * \param[in] num_de Number of Data Events.
     * \param[in,out] de_pool Pointer to the DE pool structure to be populated.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*allocate_non_ts_de_pool_shmti)(
        uint32_t num_de,
        struct mod_telemetry_de_pool *de_pool);
    /*!
     * \brief Allocate memory for a single Data Event (non-timestamped).
     *
     * \param[in,out] shmti_id Pointer to store the SHMTI identifier.
     * \param[in,out] addr Pointer to store the allocated memory address.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*allocate_de_non_ts)(uint32_t *shmti_id, uintptr_t *addr);
    /*!
     * \brief Allocate memory for a single Data Event that requires a timestamp.
     *
     * \param[in,out] shmti_id Pointer to store the SHMTI identifier.
     * \param[in,out] addr Pointer to store the allocated memory address.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*allocate_de_ts)(uint32_t *shmti_id, uintptr_t *addr);
    /*!
     * \brief Allocate a block of Data Events (DEs) sharing a common timestamp.
     *
     * \param[in] num_de Number of Data Events.
     * \param[in,out] de_pool Pointer to the DE pool structure to be populated.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*allocate_block_ts_shmti)(
        uint32_t num_de,
        struct mod_telemetry_de_pool *de_pool);
    /*!
     * \brief Free a DE pool.
     *
     * \param[in] de_pool Pointer to the DE pool structure.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*free_de_pool)(struct mod_telemetry_de_pool *de_pool);
    /*!
     * \brief Free a single Data Event.
     *
     * \param[in] shmti_id SHMTI identifier.
     * \param[in] addr Memory address of the Data Event.
     * \param[in] de_size size of the allocated Data Event.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes..
     */
    int (*free_de)(uint32_t shmti_id, uintptr_t addr, size_t de_size);
};

/*!
 * \brief API indices for Telemetry Module.
 *
 * These indices define the available APIs provided by the telemetry module.
 */
enum mod_telemetry_api_idx {
    /*! Protocol support API index */
    MOD_TELEMETRY_API_IDX_PROTOCOL_SUPPORT,

    /*! Driver support API index */
    MOD_TELEMETRY_API_IDX_DRIVER_SUPPORT,

    /*! Total number of available APIs */
    MOD_TELEMETRY_API_IDX_COUNT,
};

/*!
 * \brief API indices for Telemetry Driver.
 *
 * These indices define the available APIs for telemetry source drivers.
 */
enum mod_telemetry_driver_api_idx {
    /*! Hardware Abstraction Layer (HAL) support API index */
    MOD_TELEMETRY_DRIVER_API_IDX_HAL_SUPPORT,

    /*! Total number of available driver APIs */
    MOD_TELEMETRY_DRIVER_API_COUNT,
};

/*!
 * \brief Telemetry Driver API.
 *
 * This API allows telemetry source drivers to register data events,
 * enable/disable them, and update telemetry data.
 */
struct mod_telemetry_driver_api {
    /*!
     * \brief Get the list of registered Data Events (DEs).
     *
     * \param[out] num_de Pointer to store the number of DEs.
     * \param[out] de_list Pointer to store the DE list.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_de_list)(uint32_t *num_de, struct mod_telemetry_de **de_list);

    /*!
     * \brief Disable a specific Data Event (DE).
     *
     * \param[in] de_index Index of the DE to disable.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*disable_de)(uint32_t de_index);

    /*!
     * \brief Disable all Data Events (DEs).
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*disable_de_all)(void);

    /*!
     * \brief Enable a Data Event (DE) without an individual timestamp.
     *
     * \param[in] de_index Index of the DE to enable.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*enable_de_non_ts)(uint32_t de_index);

    /*!
     * \brief Enable a Data Event (DE) with an individual timestamp.
     *
     * \param[in] de_index Index of the DE to enable.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*enable_de_ts)(uint32_t de_index);

    /*!
     * \brief Trigger telemetry data update.
     *
     * This function updates the telemetry data by fetching values from
     * registered telemetry sources.
     */
    void (*update)();
};

/*!
 * \brief Telemetry Protocol Support API.
 *
 * This API provides protocol-level operations for managing telemetry
 * sources, data events, and configuration settings.
 */
struct mod_telemetry_protocol_support_api {
    /*!
     * \brief Get the number of Shared Memory Telemetry Instances (SHMTI).
     *
     * \param[out] num_shmti Pointer to store the number of SHMTI instances.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_num_shmti)(uint32_t *num_shmti);

    /*!
     * \brief Get details of a specific SHMTI.
     *
     * \param[in] shmti_index Index of the SHMTI.
     * \param[out] shmti_desc Pointer to store the SHMTI descriptor.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_shmti)(
        uint32_t shmti_index,
        struct mod_telemetry_shmti_desc *shmti_desc);

    /*!
     * \brief Get the total number of registered Data Events (DEs).
     *
     * \param[out] num_de Pointer to store the number of DEs.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_num_de)(uint32_t *num_de);

    /*!
     * \brief Retrieve details of a specific Data Event (DE).
     *
     * \param[in] de_index Index of the DE.
     * \param[out] de_desc Pointer to store the DE descriptor.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_de)(uint32_t de_index, struct mod_telemetry_de_desc *de_desc);

    /*!
     * \brief Get information about supported update intervals.
     *
     * \param[out] num_intervals Pointer to store the number of intervals.
     * \param[out] interval_format Pointer to store the interval format type.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_update_intervals_info)(
        uint32_t *num_intervals,
        enum mod_telemetry_update_interval_formats *interval_format);

    /*!
     * \brief Get a specific telemetry update interval.
     *
     * \param[in] interval_index Index of the interval.
     * \param[out] sampling_rate Pointer to store the sampling rate in ms.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (
        *get_update_interval)(uint32_t interval_index, uint32_t *sampling_rate);

    /*!
     * \brief Get the number of currently enabled Data Events (DEs).
     *
     * \param[out] num_de_enabled Pointer to store the number of enabled DEs.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_num_de_enabled)(uint32_t *num_de_enabled);

    /*!
     * \brief Get the status of an enabled Data Event (DE).
     *
     * \param[in] index Index of the enabled DE.
     * \param[out] de_status Pointer to store the DE status.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*get_de_enabled)(
        uint32_t index,
        struct mod_telemetry_de_status *de_status);

    /*!
     * \brief Disable all Data Events (DEs).
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*disable_all_de)(void);

    /*!
     * \brief Disable a specific Data Event (DE).
     *
     * \param[in] de_id ID of the DE to disable.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*disable_de)(uint32_t de_id);

    /*!
     * \brief Enable a Data Event (DE) without an individual timestamp.
     *
     * \param[in] de_id ID of the DE to enable.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*enable_de_non_ts)(uint32_t de_id);

    /*!
     * \brief Enable a Data Event (DE) with an individual timestamp.
     *
     * \param[in] de_id ID of the DE to enable.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*enable_de_ts)(uint32_t de_id);

    /*!
     * \brief Disable telemetry functionality.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*telemetry_disable)(void);

    /*!
     * \brief Enable telemetry functionality.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*telemetry_enable)(void);

    /*!
     * \brief Set the telemetry data sampling rate.
     *
     * \param[in] sampling_rate Sampling rate in milliseconds.
     *
     * \retval ::FWK_SUCCESS The operation was successful.
     * \return One of the standard framework error codes.
     */
    int (*set_sampling_rate)(uint32_t sampling_rate);
};
/*!
 * \}
 */

/*!
 * \}
 */
#endif /* MOD_TELEMETRY_H */
