#pragma once

#include <stddef.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/* Kernel Journal                                                             */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_sysj
 * @brief Appends a formatted entry to the kernel journal.
 *
 * @param[in] fmt Format string.
 * @param[in] ... Variable arguments corresponding to format specifiers.
 *
 * @pre
 * - fmt must not be NULL.
 *
 * @details
 * Formats a journal entry according to implementation-defined
 * formatting rules (printf-like) and appends it to the internal
 * journal buffer or log storage.
 *
 * The journal is implemented as a circular memory buffer
 *
 * Entry size limits and truncation behavior are
 * implementation-defined.
 *
 * @sideeffects
 * - Modifies internal journal buffer state.
 * - May trigger storage or console output.
 *
 * @warning
 * - Must not be called from contexts where formatting or
 *   memory allocation is unsafe (e.g., certain interrupt contexts)
 */
void _kernel_jentry(char const *fmt, ...);

/**
* @ingroup kernel_sysj
 * @brief Retrieves the next journal entry.
 *
 * @param[out] entry      Buffer receiving the journal entry string.
 * @param[in]  max_len    Maximum buffer length in bytes.
 * @param[out] entry_len  Optional pointer receiving actual entry length.
 *
 * @retval true  An entry was retrieved successfully.
 * @retval false No more entries available.
 *
 * @pre
 * - entry must not be NULL if max_len > 0.
 * - max_len > 0.
 *
 * @post
 * - entry contains a null-terminated string if true is returned.
 * - *entry_len contains the length of the entry (excluding
 *   null terminator) if entry_len is not NULL.
 *
 * @details
 * Retrieves the next available journal entry from the
 * internal journal storage.
 *
 * If the provided buffer is too small, the entry may be
 * truncated according to implementation-defined rules.
 *
 * Iteration is cursor-based.
 *
 * @sideeffects
 * - May update internal journal read cursor.
 */
bool _kernel_jnxtentry(char *entry,
                       const size_t max_len,
                       size_t *const entry_len);
