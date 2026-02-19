#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @ingroup kernel_core
 * @brief Maximum horizontal coordinate of the console.
 *
 * @details
 * Represents the maximum valid X coordinate (column index).
 * Valid cursor X positions range from 0 to (MAX_X - 1).
 *
 * @note
 * Value is implementation-defined and must be initialized
 * during console subsystem / module implementation.
 * Must be initialized.
 * When concrete console module doesnt support X coordinate navigation (e.g. tty)
 * it must be initialized as zero
 */
extern size_t MAX_X;

/**
 * @ingroup kernel_core
 * @brief Maximum vertical coordinate of the console.
 *
 * @details
 * Represents the maximum valid Y coordinate (row index).
 * Valid cursor Y positions range from 0 to (MAX_Y - 1).
 *
 * @note
 * Value is implementation-defined and must be initialized
 * during console subsystem / module implementation.
 * Must be initialized
 * When concrete console module doesnt support Y coordinate navigation (e.g. tty)
 * it must be initialized as zero
 */
extern size_t MAX_Y;

/**
 * @ingroup kernel_core
 * @brief Moves cursor to the next line.
 *
 * @details
 * Advances the cursor to the beginning of the next line.
 * Scrolling behavior is implementation-defined if the cursor
 * is currently on the last line.
 *
 * @post
 * Cursor X position is reset to 0.
 *
 * @sideeffects
 * May trigger screen scrolling.
 */
void con_ln(void);

/**
 * @ingroup kernel_core
 * @brief Inserts a horizontal tab.
 *
 * @details
 * Advances the cursor to the next tab stop.
 * Tab width is implementation-defined.
 *
 * @sideeffects
 * Modifies cursor position.
 */
void con_tab(void);

/**
 * @ingroup kernel_core
 * @brief Clears the console display.
 *
 * @details
 * Clears the entire screen buffer and resets the cursor
 * to the origin position (0,0).
 *
 * @post
 * - Screen buffer is cleared.
 * - Cursor position is (0,0).
 *
 * @note
 * When concrete console module doesnt support screen buffer clear (e.g. tty)
 * it can be empty implemented. e.g. void con_clear() { }
 *
 * @sideeffects
 * Modifies the entire display buffer.
 */
void con_clear(void);

/**
 * @ingroup kernel_core
 * @brief Writes a single character to the console.
 *
 * @param[in] code Character to display.
 *
 * @details
 * Writes the specified character at the current cursor position
 * and advances the cursor according to implementation rules.
 *
 * @note
 * Control characters may be handled specially.
 *
 * @sideeffects
 * - Modifies display buffer.
 * - Updates cursor position.
 */
void con_char(const char code);

/**
 * @ingroup kernel_core
 * @brief Writes a null-terminated string to the console.
 *
 * @param[in] str Pointer to null-terminated character string.
 *
 * @pre
 * str must not be NULL.
 *
 * @details
 * Writes characters sequentially starting at the current cursor
 * position. Cursor advancement and wrapping behavior are
 * implementation-defined.
 *
 * @sideeffects
 * - Modifies display buffer.
 * - Updates cursor position.
 */
void con_string(const char* str);

/**
 * @ingroup kernel_core
 * @brief Sets the cursor position.
 *
 * @param[in] cx Horizontal position (column index).
 * @param[in] cy Vertical position (row index).
 *
 * @pre
 * - cx < MAX_X
 * - cy < MAX_Y
 *
 * @details
 * Moves the cursor to the specified coordinates.
 * Behavior is undefined if parameters exceed console bounds
 * unless explicitly handled by implementation.
 *
 * @note
 * When console doesnt support coordinates navigation (e.g. tty)
 * it can be empty implemented e.g. void con_xy(size_t x, size_t y) { }
 *
 * @sideeffects
 * Modifies internal cursor state.
 */
void con_xy(size_t cx, size_t cy);

/**
 * @ingroup kernel_core
 * @brief Retrieves the current cursor X position.
 *
 * @return Current horizontal cursor position.
 *
 * @details
 * Returns a value in the range [0, MAX_X - 1].
 */
size_t con_getx(void);

/**
 * @ingroup kernel_core
 * @brief Retrieves the current cursor Y position.
 *
 * @return Current vertical cursor position.
 *
 * @details
 * Returns a value in the range [0, MAX_Y - 1].
 */
size_t con_gety(void);

/**
 * @ingroup kernel_core
 * @brief Initializes the console subsystem.
 *
 * @details
 * Performs required hardware or buffer initialization and
 * prepares the console for output operations.
 *
 * Must be called before using any other console function.
 *
 * @post
 * - Console is ready for output.
 * - MAX_X and MAX_Y are initialized.
 *
 * @sideeffects
 * - Initializes internal state.
 * - May access hardware registers or memory-mapped I/O.
 */
void set_con(void);
