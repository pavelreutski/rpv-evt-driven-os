#pragma once

#include "events.h"

/**
 * @ingroup kernel_core
 * @brief Executes the kernel event pipeline.
 *
 * @details
 * Processes all pending events in the kernel event queue,
 * dispatching them to registered subscribers.
 *
 * This function is typically called in the main kernel loop
 * or from a dedicated event task.
 *
 * @sideeffects
 * - Invokes subscriber callbacks for pending events.
 * - May modify internal kernel event queue state.
 *
 * @warning
 * - Subscribers must be reentrant if pipeline is executed
 *   from multiple contexts.
 * - Long-running callbacks may delay event processing.
 */
void _kernel_pipeline(void);

/**
 * @ingroup kernel_core
 * @brief Publishes an event to the kernel event system.
 *
 * @param[in] id   Event identifier (kernel or user event code).
 * @param[in] data Pointer to event data payload.
 *
 * @pre
 * - data may be NULL if the event has no payload.
 *
 * @details
 * Adds the event to the kernel event queue for later processing
 * by `_kernel_pipeline()`.
 *
 * @sideeffects
 * - Updates internal kernel event queue.
 * - May trigger deferred execution of subscriber callbacks.
 *
 * @note
 * - Publisher might not ensure that evt_data_t remains valid until
 *   the event is processed because a kernel makes a deep copy of the payload at the event enque stage
 */
void _kernel_pubEvt(uint8_t id, evt_data_t* data);

/**
 * @ingroup kernel_core
 * @brief Registers a subscriber callback for a specific event.
 *
 * @param[in] id         Event identifier to subscribe to.
 * @param[in] subscriber Function pointer to callback invoked
 *                       when the event is published.
 *
 * @pre
 * - subscriber must not be NULL.
 *
 * @details
 * Adds the subscriber to the internal subscription list
 * for the specified event.
 *
 * @sideeffects
 * - Modifies internal kernel subscription table.
 *
 * @warning
 * - Subscriber execution can be blocking but should be time bounded.
 */
void _kernel_subEvt(uint8_t id, evt_subscriber_t subscriber);
