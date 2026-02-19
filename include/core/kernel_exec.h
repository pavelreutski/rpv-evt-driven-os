#pragma once

/**
 * @ingroup kernel_core
 * @brief Executes a function in kernel context with argument forwarding.
 *
 * @param[in] exec_func Pointer to the function to execute.
 *                      Must match the signature `int func(int argc, const char **argv)`.
 * @param[in] argc      Number of arguments to pass to the function.
 * @param[in] argv      Array of null-terminated argument strings.
 *
 * @pre
 * - exec_func must not be NULL.
 * - argv must not be NULL if argc > 0.  
 *
 * @details
 * This function provides a safe wrapper to execute a user-supplied
 * function within kernel or privileged context, forwarding the
 * specified arguments.
 *
 * The provided function is expected to:
 * - Return an integer status code.
 * - Handle the arguments correctly.
 *
 * @post
 * - The exec_func is invoked with the specified arguments.
 *
 * @sideeffects
 * - Executes arbitrary code in kernel context; may modify
 *   global or hardware state depending on exec_func implementation.
 *
 * @warning
 * - exec_func must be reentrant if called from concurrent contexts.
 * - Misbehaving functions may compromise kernel stability.
 */
void _kernel_exec_f(
		int (*exec_func)(const int argc, const char **argv),
		const int argc, const char **argv);
