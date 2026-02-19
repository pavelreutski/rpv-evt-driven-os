#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/* Kernel Filesystem I/O Initialization                                       */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_fio
 * @brief Initializes kernel file I/O subsystem.
 *
 * @details
 * Prepares internal filesystem structures, mount tables,
 * and underlying block device interfaces.
 *
 * Must be called before invoking mount, unmount, or
 * drive-related functions.
 *
 * @post
 * - Filesystem subsystem is ready.
 * - Internal drive state is initialized.
 *
 * @sideeffects
 * - Initializes internal static state.
 * - May access storage hardware.
 */
void _kernel_fio(void);

/* -------------------------------------------------------------------------- */
/* Drive Management                                                           */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_fio
 * @brief Retrieves current active drive letter.
 *
 * @return
 * Drive letter of the currently mounted filesystem.
 *
 * @retval 0 No drive mounted.
 *
 * @details
 * Returns the logical drive identifier (e.g., 'A', 'B', 'C').
 */
char _kernel_cdrive(void);

/**
 * @ingroup kernel_fio
 * @brief Unmounts the currently mounted drive.
 *
 * @retval true  Unmount successful.
 * @retval false Unmount failed or no drive mounted.
 *
 * @post
 * - No active filesystem is mounted on success.
 *
 * @sideeffects
 * - Flushes pending filesystem operations.
 * - Updates internal mount state.
 */
bool _kernel_unmount(void);

/**
 * @ingroup kernel_fio
 * @brief Mounts a drive by letter.
 *
 * @param[in] letter Logical drive letter to mount.
 *
 * @retval true  Mount successful.
 * @retval false Mount failed.
 *
 * @pre
 * - letter must correspond to a valid available drive.
 *
 * @post
 * - Specified drive becomes active on success.
 *
 * @sideeffects
 * - Initializes filesystem structures for the drive.
 */
bool _kernel_mount(char letter);

/**
 * @ingroup kernel_fio
 * @brief Retrieves available drive letters.
 *
 * @param[out] drive_lt Buffer receiving drive letters.
 * @param[in]  nlt      Maximum number of letters to store.
 *
 * @return
 * Number of drive letters written to buffer.
 *
 * @pre
 * - drive_lt must not be NULL if nlt > 0.
 *
 * @details
 * Writes up to nlt available drive identifiers into the
 * provided buffer. Letters are not null-terminated unless
 * explicitly handled by the caller.
 *
 * @note
 * If the number of available drives exceeds nlt,
 * the list is truncated.
 */
size_t _kernel_drives(char *drive_lt, size_t nlt);
