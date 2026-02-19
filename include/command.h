#pragma once

#include <stdint.h>

/**
 * @ingroup kernel_core
 * @brief Command descriptor structure
 *
 * @details
 * Represents a shell command entry stored in the command table.
 * Instances of this structure are typically placed in a dedicated
 * linker section and discovered at runtime by the shell subsystem.
 *
 * The structure binds a command name to its handler function.
 *
 * @note
 * Objects of this type are expected to be constant and
 * located in read-only memory.
 */
struct command_s {

    /**
     * @brief Null-terminated command name string.
     *
     * @details
     * Must point to a valid constant string.
     * The string must remain valid for the lifetime of the program.
     */
    const char* text;

    /**
     * @brief Command handler function pointer.
     *
     * @details
     * Function invoked when the command is executed.
     *
     * @param[in] argc  Number of arguments in argv.
     * @param[in] argv  Array of argument strings.
     *
     * @return
     * Status code returned by the command handler.
     *
     * @retval 0        Command executed successfully.
     * @retval <0       Command failed (implementation-defined).
     *
     * @pre
     * - argv always not NULL.
     * - argc >= 1. When 1 argv just contains an exact typed command name
     * - Handler must be reentrant if used in concurrent context.
     */
    int (*const handler)(const int argc, const char **argv);
};

/**
 * @ingroup kernel_core
 * @brief Command descriptor type alias.
 */
typedef struct command_s command_t;

/**
 * @ingroup kernel_core
 * @brief Declares and registers a shell command.
 *
 * @details
 * Defines a static constant command_t instance and places it in
 * the ".cmd_table" linker section.
 *
 * The linker aggregates all such entries into a contiguous table
 * used by the shell subsystem for command lookup.
 *
 * @param name
 * Identifier of the command object.
 * The same identifier string is used as the command name.
 *
 * @param callback
 * Pointer to the command handler function.
 *
 * @note
 * - The command object is marked as used to prevent removal by
 *   linker garbage collection.
 * - Requires linker script support for ".cmd_table" section.
 *
 * @warning
 * The handler function must match the required prototype:
 * int handler(const int argc, const char **argv);
 *
 * @par Linker Requirements
 * The linker script must define start and end symbols for the
 * ".cmd_table" section to enable iteration.
 *
 * @par Example
 * @code
 * static int cmd_hello(const int argc, const char **argv)
 * {
 *     (void)argc;
 *     (void)argv;
 *     return 0;
 * }
 *
 * _SHELL_COMMAND(hello, cmd_hello);
 * @endcode
 */
#define _SHELL_COMMAND(name, callback) \
    __attribute__((used, section(".cmd_table"))) \
    static const command_t name = { #name, callback }
