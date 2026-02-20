/**
 * @file disk.h
 * @ingroup kernel_needimpl
 * @brief Disk low level I/O interface called by file I/O subsystem
 */
 
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Disk I/O status codes.
 *
 * @details
 * Return values used by disk I/O operations to indicate
 * success or failure conditions.
 *
 * @note
 * Values are ordered but shall not be relied upon for
 * numeric comparison unless explicitly documented.
 */
enum
{
    DISK_IO_OK,        /**< Operation completed successfully. */
    DISK_IO_ERR,       /**< General I/O error occurred. */
    DISK_INVALID,      /**< Invalid parameter or disk identifier. */
    DISK_NOT_READY,    /**< Disk is not initialized or not ready. */
    DISK_WRITE_PROTECT /**< Write operation attempted on protected media. */
};

/**
 * @brief Disk descriptor structure.
 *
 * @details
 * Contains metadata describing a physical or logical disk device.
 * Instances of this structure are populated by the file i/o subsystem.
 *
 * @note
 * This structure does not contain runtime state or buffers.
 */
typedef struct
{
    /**
     * @brief Logical disk letter (e.g., 'A', 'B', 'C').
     */
    char letter;

    /**
     * @brief Internal disk identifier.
     */
    size_t did;

    /**
     * @brief Volume size.
     */
    size_t volume;

    /**
     * @brief Total number of addressable blocks.
     */
    size_t num_blocks;

    /**
     * @brief Size of a single block in bytes.
     */
    size_t block_size;

} disk_t;

/**
 * @ingroup kernel_needimpl
 * @brief Initializes the disk I/O module be a part of a file I/O subsystem.
 *
 * @details
 * Performs hardware detection, driver initialization,
 * and prepares internal file I/O subsystem disk tables.
 *
 * Must be called before invoking other disk API functions.
 *
 * @post
 * Disk I/O is ready for operations.
 *
 * @sideeffects
 * - May access hardware.
 * - Initializes internal static state.
 */
void disk_io(void);

/**
 * @ingroup kernel_needimpl
 * @brief Returns the number of detected disks.
 *
 * @return
 * Number of available disk devices.
 *
 * @retval 0 No disks detected or module not initialized.
 */
size_t get_disks(void);

/**
 * @ingroup kernel_needimpl
 * @brief Retrieves disk descriptor by identifier.
 *
 * @param[in]  did   Disk identifier.
 * @param[out] disk  Pointer to disk descriptor structure.
 *
 * @pre
 * - disk must not be NULL.
 * - did must refer to a valid disk.
 *
 * @post
 * disk structure is populated on success.
 *
 * @warning
 * Behavior is undefined if disk is NULL.
 */
void get_disk(size_t did, disk_t *disk);

/**
 * @ingroup kernel_needimpl
 * @brief Retrieves human-readable disk information string.
 *
 * @param[in]  did           Disk identifier.
 * @param[out] dsk_info      Output buffer for disk information string.
 * @param[in]  dsk_infoLen   Length of output buffer in bytes.
 *
 * @pre
 * - dsk_info must not be NULL.
 * - dsk_infoLen > 0.
 *
 * @details
 * Writes a null-terminated string describing the disk.
 * Output is truncated if buffer is insufficient.
 *
 * @sideeffects
 * Modifies dsk_info buffer.
 */
void get_diskInfo(size_t did, char *dsk_info, size_t dsk_infoLen);

/**
 * @ingroup kernel_needimpl
 * @brief Flushes pending write operations to disk.
 *
 * @param[in] did Disk identifier.
 *
 * @return
 * Disk I/O status code.
 *
 * @retval DISK_IO_OK        Flush successful.
 * @retval DISK_INVALID      Invalid disk identifier parameter
 * @retval DISK_NOT_READY    Disk not initialized.
 * @retval DISK_IO_ERR       Hardware error occurred.
 *
 * @sideeffects
 * May trigger physical media write.
 */
uint8_t flush_disk(size_t did);

/**
 * @ingroup kernel_needimpl
 * @brief Reads blocks from disk.
 *
 * @param[out] buffer Destination buffer.
 * @param[in]  did    Disk identifier.
 * @param[in]  blck   Starting block index.
 * @param[in]  blocks Number of blocks to read.
 *
 * @return
 * Disk I/O status code.
 *
 * @retval DISK_IO_OK        Read successful.
 * @retval DISK_INVALID      Invalid parameters.
 * @retval DISK_NOT_READY    Disk not initialized.
 * @retval DISK_IO_ERR       Hardware read failure.
 *
 * @pre
 * - buffer must not be NULL.
 * - blocks > 0.
 * - blck + blocks must not exceed disk capacity.
 *
 * @sideeffects
 * Modifies contents of buffer.
 */
uint8_t read_disk(void *buffer, size_t did, size_t blck, size_t blocks);

/**
 * @ingroup kernel_needimpl
 * @brief Writes blocks to disk.
 *
 * @param[in] buffer Source buffer.
 * @param[in] did    Disk identifier.
 * @param[in] blck   Starting block index.
 * @param[in] blocks Number of blocks to write.
 *
 * @return
 * Disk I/O status code.
 *
 * @retval DISK_IO_OK          Write successful.
 * @retval DISK_WRITE_PROTECT  Media is write-protected.
 * @retval DISK_INVALID        Invalid parameters.
 * @retval DISK_NOT_READY      Disk not initialized or media not ready
 * @retval DISK_IO_ERR         Hardware write failure.
 *
 * @pre
 * - buffer must not be NULL.
 * - blocks > 0.
 * - blck + blocks must not exceed disk capacity.
 *
 * @sideeffects
 * - Modifies disk media.
 * - May trigger physical write operation.
 *
 * @warning
 * Data integrity is not guaranteed unless flush_disk()
 * is called after write operations when required.
 */
uint8_t write_disk(void const* buffer, size_t did, size_t blck, size_t blocks);
