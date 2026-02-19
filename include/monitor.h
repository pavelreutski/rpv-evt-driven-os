#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @ingroup kernel_core
 * @brief Outputs a null-terminated text string to the console.
 *
 * @param[in] s Pointer to a null-terminated character string.
 *
 * @return
 * Number of characters written to the console.
 *
 * @retval 0 No characters written (e.g., s is NULL or empty string).
 *
 * @pre
 * - s must not be NULL.
 *
 * @details
 * Writes the provided string to the console output interface. 
 * The output device is implementation-defined
 * (e.g., tty, VGA text mode, debug console).
 *
 * The function stops writing upon encountering the null terminator.
 *
 * @sideeffects
 * - Produces output on console interface (see console.h).
 *
 */
size_t text_monitor(char const* s);

/**
 * @ingroup kernel_core
 * @brief Outputs a memory region as hexadecimal values to the console.
 *
 * @param[in] mem Pointer to memory buffer.
 * @param[in] s   Size of memory region in bytes.
 *
 * @return
 * Number of bytes processed and written in hexadecimal form.
 *
 * @retval 0 No output generated (e.g., mem is NULL or s == 0).
 *
 * @pre
 * - mem must not be NULL.
 * - s > 0.
 *
 * @details
 * Reads the specified memory region and writes its contents
 * to the console formatted as hexadecimal values.
 *
 * Output formatting (grouping, spacing, line width) is
 * implementation-defined.
 *
 * @sideeffects
 * - Produces formatted diagnostic output.
 *
 * @warning
 * Caller must ensure that the memory region is valid and
 * accessible. Undefined behavior may occur if invalid memory
 * is provided or there is no region access fault / exception handler exists
 *
 */
size_t hex_monitor(void const* mem, const size_t s);
