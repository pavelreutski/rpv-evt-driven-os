/**
 * @ingroup kernel_core
 * @brief Kernel version major number.
 *
 * @details
 * Indicates the major version of the kernel.
 * Incremented for incompatible API changes or major feature updates.
 */
#define MAJOR                                (1)

/**
 * @ingroup kernel_core
 * @brief Kernel version minor number.
 *
 * @details
 * Indicates the minor version of the kernel.
 * Incremented for backward-compatible functionality additions.
 */
#define MINOR                                (0)

/**
 * @ingroup kernel_core
 * @brief Kernel patch number.
 *
 * @details
 * Indicates the patch or bugfix level.
 * Incremented for backward-compatible bug fixes or minor improvements.
 */
#define PATCH                                (0)

/**
 * @ingroup kernel_core
 * @brief Kernel revision label.
 *
 * @details
 * Provides an optional string indicating the release stage
 * (e.g., "alpha", "beta", "rc1", "stable").
 *
 * @note
 * Useful for pre-release or development builds.
 */
#define REVISION							 ("alpha")
