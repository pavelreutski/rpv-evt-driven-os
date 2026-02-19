#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "events.h"

/* -------------------------------------------------------------------------- */
/* Kernel STDIO Initialization                                                */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Initializes kernel standard I/O subsystem.
 *
 * @details
 * Prepares console output, input handling, and any required
 * internal buffers. Must be invoked before using any
 * _kernel_out* or input-related functions.
 *
 * @post
 * - Console output is operational.
 * - Keyboard/input subsystem is ready.
 *
 * @sideeffects
 * - Initializes internal state.
 * - May register event subscribers.
 */
void _kernel_stdio(void);

/* -------------------------------------------------------------------------- */
/* Output Functions                                                           */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Outputs a newline sequence.
 *
 * @details
 * Advances output cursor to the next line.
 */
void _kernel_outLn(void);

/**
 * @ingroup kernel_core
 * @brief Outputs a horizontal tab.
 *
 * @details
 * Advances output cursor to the next tab stop.
 */
void _kernel_outTab(void);

/**
 * @ingroup kernel_core
 * @brief Outputs a single character.
 *
 * @param[in] code Character to output.
 *
 * @sideeffects
 * Writes to console or debug output device.
 */
void _kernel_outChar(const char code);

/**
 * @ingroup kernel_core
 * @brief Outputs a null-terminated string.
 *
 * @param[in] str String to output.
 *
 * @pre str must not be NULL.
 */
void _kernel_outString(const char *str);

/**
 * @ingroup kernel_core
 * @brief Outputs a formatted string.
 *
 * @param[in] fmt Format string.
 * @param[in] ... Variable arguments.
 *
 * @pre fmt must not be NULL.
 *
 * @details
 * Formats the string according to implementation-defined
 * formatting rules (printf-like) and writes it to output.
 *
 * @warning
 * Supported format specifiers are implementation-defined.
 */
void _kernel_outStringFormat(const char *fmt, ...);

/* -------------------------------------------------------------------------- */
/* String Formatting                                                          */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Formats a string into a buffer.
 *
 * @param[out] s   Output buffer.
 * @param[in]  fmt Format string.
 * @param[in]  ... Variable arguments.
 *
 * @pre
 * - s must not be NULL.
 * - fmt must not be NULL.
 *
 * @details
 * Writes formatted output into the provided buffer.
 * Buffer size management is implementation-defined.
 */
void _kernel_stringFormat(char *s, char const *fmt, ...);

/**
 * @ingroup kernel_core
 * @brief Formats a string using a va_list argument.
 *
 * @param[out] s    Output buffer.
 * @param[in]  fmt  Format string.
 * @param[in]  args Argument list.
 *
 * @pre
 * - s must not be NULL.
 * - fmt must not be NULL.
 *
 * @details
 * Equivalent to _kernel_stringFormat() but accepts a prebuilt
 * va_list. Intended for internal use or wrapper functions.
 */
void _kernel_vstringFormat(char *s,
                           char const *fmt,
                           va_list args);

/* -------------------------------------------------------------------------- */
/* Input Functions                                                            */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_core
 * @brief Retrieves a key event (blocking).
 *
 * @param[out] key Structure receiving key information.
 *
 * @pre key must not be NULL.
 *
 * @details
 * Blocks until a console key event is available.
 * Populates the provided console_key_t structure.
 *
 * @warning
 * May block calling context indefinitely.
 */
void _kernel_getKey(console_key_t *key);

/**
 * @ingroup kernel_core
 * @brief Attempts to retrieve a key event (non-blocking).
 *
 * @param[out] key Structure receiving key information.
 *
 * @retval true  A key event was retrieved.
 * @retval false No key available.
 *
 * @pre key must not be NULL.
 *
 * @details
 * Non-blocking version of _kernel_getKey().
 */
bool _kernel_tryGetKey(console_key_t *key);
