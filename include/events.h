#pragma once

#include <stdint.h>

/// @brief defines user events in kernel (extendable by last member)
enum user_events_e {

	EVT_USERCON_KEY = 50,	
	EVT_SYS_USER_EVENTS
};

/// @brief define kernel events (not accessible for user subscriptions)
enum kernel_events_e {

	EVT_ONESHOT_TICK,
	EVT_PERIODIC_TICK,
	EVT_KERNEL_DTC,
	EVT_KERNEL_EVENTS
};

struct console_key_s {

	char code;
	uint8_t scan_code;
};

union evt_data_u {
	
	struct console_key_s con_key;
};

typedef union evt_data_u evt_data_t;
typedef struct console_key_s console_key_t;

typedef void (*evt_subscriber_t)(evt_data_t* evtData);