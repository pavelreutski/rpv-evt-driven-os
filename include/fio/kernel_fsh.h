#pragma once

/* -------------------------------------------------------------------------- */
/* Kernel Filesystem Command Shell                                            */
/* -------------------------------------------------------------------------- */

/**
 * @ingroup kernel_fio
 * @brief Changes the current working directory.
 *
 * @param[in] path Target directory path.
 *
 * @pre
 * - path must not be NULL.
 *
 * @details
 * Changes the active working directory within the
 * kernel filesystem context.
 *
 * Behavior on invalid path is implementation-defined
 * (e.g., error message output, silent failure, etc.).
 *
 * @sideeffects
 * - Updates current working directory state.
 *
 * @note
 * Path resolution rules: absolute and relative.
 */
void _kernel_cd(char const *path);

/**
 * @ingroup kernel_fio
 * @brief Creates a new directory.
 *
 * @param[in] path Directory path to create.
 *
 * @pre
 * - path must not be NULL.
 *
 * @details
 * Creates a directory at the specified path.
 *
 * @sideeffects
 * - Modifies filesystem metadata.
 */
void _kernel_mkdir(char const *path);

/**
 * @ingroup kernel_fio
 * @brief Removes a file.
 *
 * @param[in] path Path to file to remove.
 *
 * @pre
 * - path must not be NULL.
 *
 * @details
 * Deletes the specified file from the filesystem.
 *
 * Removal of directories using this function when not empty
 * will result in access denied.
 *
 * @sideeffects
 * - Modifies filesystem structures.
 *
 * @warning
 * Operation is irreversible.
 */
void _kernel_rmfile(char const *path);

/**
 * @ingroup kernel_fio
 * @brief Lists directory contents.
 *
 * @param[in] argc Number of arguments.
 * @param[in] argv Argument vector.
 *
 * @pre
 * - argv must not be NULL if argc > 0.
 *
 * @details
 * Implements directory listing functionality.
 * Arguments may include:
 * - Target path
 * - Optional flags.
 *
 * Intended for invocation from kernel shell.
 *
 * @sideeffects
 * - Produces formatted output to console.
 */
void _kernel_ls(const int argc, const char **argv);
