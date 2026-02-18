#include <string.h>

#include "kernel_conf.h"

#include "private/command_stack.h"

#define COMMAND(cmd_ptr)             (cmd_ptr % MAX_BUFFER_COMMANDS)

static uint8_t stack_cmd             = 0;
static uint8_t stack_commands        = 0;

static uint8_t stack_counter         = 0;

static char cmd_stack[MAX_BUFFER_COMMANDS][MAX_COMMAND_BUFFER];

void push_stackCommand(char *cmd) {

	if ((*cmd) == '\0')  {
		return;
	}

	char *buffer = cmd_stack[COMMAND(stack_counter)];

	strcpy(buffer, cmd);

	stack_cmd = ++stack_counter;

	if (stack_commands < MAX_BUFFER_COMMANDS) {
		stack_commands++;
	}
}

bool peak_stackCommand(char *cmd, cmd_stack_cursor_t cursor) {

	if ((cmd == NULL) || (stack_commands == 0)) {
		return false; 
	}

	switch(cursor) {

		case COMMAND_STACK_NEXT: {

			if (!(stack_cmd ^ stack_counter) |
					!(stack_cmd ^ (stack_counter - 1)))
				return false;

			stack_cmd++;

		} break;

		case COMMAND_STACK_PREV: {

			if (!(stack_cmd ^ (stack_counter - stack_commands)))
				return false;

			stack_cmd--;

		} break;

		default: return false;
	}

	char *buffer = cmd_stack[COMMAND(stack_cmd)];

	strcpy(cmd, buffer);
	return true;
}