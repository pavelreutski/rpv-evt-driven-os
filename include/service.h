#pragma once

/**
 * @ingroup kernel_core
 * @brief Service descriptor structure.
 *
 * @details
 * Represents a statically registered system service.
 * Service instances are typically placed in a dedicated
 * linker section and discovered during system services runtime.
 *
 * Each service provides:
 * - A unique service name
 * - An entry function (svc_main)
 *
 * @note
 * Service objects are expected to reside in read-only memory.
 */
struct service_s {

    /**
     * @brief Null-terminated service name string.
     *
     * @details
     * Identifies the service. Must remain valid for the lifetime
     * of the system.
     */
    const char *s_name;

    /**
     * @brief Service entry function.
     *
     * @details
     * Main execution function of the service.
     *
     * @pre
     * - Must not be NULL.
     * - Must not return unless explicitly allowed by design.
     *
     * @warning
     * When executed in kernel context,
     * the implementation must be bounded and non-blocking
     * unless explicitly designed as a long-running service.
     * The kernel invokes svc_main() periodically. The periodicity is kernel scheduler implementation defined
     */
    void (*const svc_main)(void);
};

/**
 * @ingroup kernel_core
 * @brief Service descriptor alias
 */
typedef struct service_s service_t;

/**
 * @ingroup kernel_core
 * @brief Function pointer type for service initialization routines.
 *
 * @details
 * Defines the signature of a function invoked once during kernel
 * startup to initialize a system service.
 *
 * The function takes no arguments and returns no value.
 *
 * @note
 * Service initialization functions are registered using
 * @ref _SERVICE_INIT.
 */
typedef void (*service_init_t)(void);

/**
 * @ingroup kernel_core
 * @brief Declares and registers a system service.
 *
 * @param name         Identifier of the service object.
 *                     The same identifier string is used as
 *                     the service name.
 *
 * @param service_main Pointer to the service entry function.
 *
 * @details
 * Defines a static constant service_t instance and places it
 * into the ".service_table" linker section.
 *
 * The linker aggregates all registered services into a
 * contiguous table that can be iterated during system services runtime.
 *
 * The object is marked with the "used" attribute to prevent
 * removal by linker garbage collection.
 *
 * @warning
 * Requires linker script support defining the ".service_table"
 * section and its boundary symbols.
 *
 * @par Example
 * @code
 * static void logger_service(void)
 * {
 *     // Service implementation
 * }
 *
 * _SERVICE(logger, logger_service);
 * @endcode
 *
 * @par Linker Requirements
 * The linker script must expose start/end symbols for
 * ".service_table" to allow iteration.
 */
#define _SERVICE(name, service_main) \
    __attribute__((used, section(".service_table"))) \
    static const service_t name = { #name, service_main }

/**
 * @ingroup kernel_core
 * @brief Registers a service initialization function.
 *
 * @param name Identifier of the initialization entry.
 * @param service_main Initialization function.
 *
 * @details
 * Places the initialization function address into the
 * ".service_init" linker section.
 *
 * The kernel invokes all registered initialization functions
 * once during system startup, before entering the service
 * runtime loop.
 *
 * Initialization functions must return and should perform only
 * startup/initialization work.
 *
 * @warning
 * Requires linker script support for the ".service_init" section
 * and its boundary symbols.
 *
 * @par Example
 * @code
 * static void logger_init(void)
 * {
 *     // Initialize logger.
 * }
 *
 * _SERVICE_INIT(logger_startup, logger_init);
 * @endcode
 */
#define _SERVICE_INIT(name, service_main) \
    __attribute__((used, section(".service_init"))) \
    static const service_init_t name = service_main
