#pragma once

#include "events.h"

/// @brief executes kernel processing pipeline
/// @param none
void _kernel_pipeline(void);

void _kernel_pubEvt(uint8_t id, evt_data_t* data);
void _kernel_subEvt(uint8_t id, evt_subscriber_t subscriber);