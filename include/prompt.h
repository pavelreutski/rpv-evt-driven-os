#pragma once

#include <stddef.h>

/**
 * @ingroup kernel_core
 * @brief Generates or retrieves the current shell prompt string.
 *
 * @param[out] prompt  Output buffer receiving the prompt string.
 * @param[in]  p_len   Length of the output buffer in bytes.
 *
 * @pre
 * - prompt must not be NULL.
 * - p_len > 0.
 *
 * @post
 * - prompt contains a null-terminated string on success.
 * - The string is truncated if the buffer is insufficient.
 *
 * @details
 * Writes the current shell prompt into the provided buffer.
 * The prompt format is implementation-defined and may include
 * contextual information such as:
 * - Current working directory
 * - User name
 * - System state indicators
 *
 * The function guarantees null-termination provided p_len > 0.
 *
 * @sideeffects
 * - None expected (read-only state access).
 *
 * @warning
 * The caller is responsible for providing a sufficiently large
 * buffer to avoid truncation.
 *
 */
void _shell_prompt(char *prompt, const size_t p_len);
