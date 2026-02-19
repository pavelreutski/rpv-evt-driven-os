#pragma once

/**
 * @ingroup kernel_core
 * @brief Retrieves next kernel service descriptor.
 *
 * @param[in]  svc       Opaque pointer used for service table iteration.
 *                       May be NULL to retrieve the first service.
 *                       Then passing a service descriptor shall return a next one or NULL
 * @param[out] svc_name  Pointer receiving the service name.
 *
 * @return
 * Pointer to a service descriptor or NULL if no further
 * services are available or lookup fails.
 *
 * @pre
 * - svc_name must be a valid writable pointer.
 *
 * @details
 * Provides access to statically registered kernel services,
 * typically stored in a dedicated linker section.
 *
 * This function may be used to:
 * - Iterate through all registered services
 * - Retrieve metadata associated with a service
 *
 * The iteration mechanism is implementation-defined. Commonly,
 * callers pass NULL to obtain the first service and then pass
 * the previously returned pointer to retrieve the next one.
 *
 * @post
 * - If a non-NULL value is returned *svc_name points to a valid null-terminated string.
 *
 * @sideeffects
 * - None expected (read-only access to service table).
 *
 * @warning
 * - Returned pointer refers to statically allocated memory.
 * - Must not be modified by the caller.
 */
void const* _kernel_service(void const* svc, char const** svc_name);
