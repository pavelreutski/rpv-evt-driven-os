#pragma once

#include <stdint.h>
#include <stdbool.h>

enum cmd_cursor_e {
	
	COMMAND_STACK_NEXT,
	COMMAND_STACK_PREV
};

typedef enum cmd_cursor_e cmd_stack_cursor_t;

void push_stackCommand(char *cmd);
bool peak_stackCommand(char *cmd, cmd_stack_cursor_t cursor);
