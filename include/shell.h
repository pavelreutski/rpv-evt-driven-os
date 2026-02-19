#pragma once

/**
 * @ingroup kernel_core
 * @brief Starts the system shell.
 *
 * @details
 * Initializes and launches the shell or command-line interface
 * subsystem. Typically, this function:
 * - Initializes shell state and buffers
 * - Displays the initial prompt
 * - Enters the main shell execution loop
 *
 * @pre
 * - Underlying console or I/O system must be initialized.
 * - Any required system services or drivers must be ready.
 *
 * @post
 * - Control enters the shell loop and remains there until
 *   the shell exits or the system shuts down.
 *
 * @sideeffects
 * - May modify console state and internal shell buffers.
 * - Starts event handling for shell commands.
 *
 * @warning
 * - This function typically does not return under normal operation.
 * - Must not be called before system initialization is complete.
 */
void _shell_start(void);
