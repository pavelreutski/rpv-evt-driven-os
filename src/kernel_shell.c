#include <string.h>
#include <stdlib.h>

#include <stdint.h>
#include <stdbool.h>

#include "shell.h"
#include "prompt.h"
#include "command.h"

#include "kernel.h"
#include "kernel_stdio.h"

#include "kernel_conf.h"

#include "private/command_stack.h"

// ---- Messages

#define CODE	             					 ("code")

#define BAD_COMMAND_MSG                          ("Bad command or file name\n")
#define BAD_EXEC_FORMAT_MSG                      ("Bad executable image format\n")

#define CMD_SHELL_COPYRIGHT_MSG                  ("(C)RPV Co 2004-2026.\n")
#define CMD_SHELL_WELCOME_MSG                    ("RPVCo(R) Command Shell Program\n")

static int ret_code                               = 0;
static uint8_t cmd_length                         = 0;

static char cmd_buffer[MAX_COMMAND_BUFFER];
static char *cmd_args[MAX_COMMAND_ARGS]           = { cmd_buffer };

// private internal calls

static int shell_command(void);

static void display_prompt(void);
static void display_stackCommand(uint8_t direction);

static void parse_cmdArgs(char* input, int length, char **argv, int *argc);

extern command_t __cmd_table[];
extern command_t __end_cmd_table[];

// never returns

void _shell_start() {
	
	_kernel_outLn();
	_kernel_outTab();

	_kernel_outString(CMD_SHELL_WELCOME_MSG);

	_kernel_outLn();
	_kernel_outTab();

	_kernel_outString(CMD_SHELL_COPYRIGHT_MSG);

	_kernel_outLn();
	_kernel_outLn();

	display_prompt();

	memset(cmd_buffer, 0, sizeof(cmd_buffer));

	while(true) {

		console_key_t key;
		_kernel_getKey(&key);

		switch(key.scan_code) {

			// Handle scan codes for the control keys e.g. arrow keys

			case 0x75: // arrow UP
				display_stackCommand(COMMAND_STACK_PREV); break;
			case 0x72:// arrow DOWN
				display_stackCommand(COMMAND_STACK_NEXT); break;
			default: {

				// Handle ASCII code

				switch(key.code) {

					case '\b': {

						if (!cmd_length) break;

						cmd_length--;
						_kernel_outChar(0x7f); // ASCII DEL

					} break;

					case '\n': {

						// make command being in ASCIIZ format (NULL terminated)
						cmd_buffer[cmd_length] = 0;
						
						push_stackCommand(cmd_buffer);

						_kernel_outLn();
						ret_code = shell_command(); // try execute currently typed command

						display_prompt();
						cmd_length ^= cmd_length;

					} break;

					default: {

						if (!(key.code)) break;
						if (!(cmd_length ^ MAX_COMMAND_BUFFER)) break;

						_kernel_outChar(key.code);
						cmd_buffer[cmd_length++] = key.code;

					} break;
				}

			} break;
		}
	}
}

static void parse_cmdArgs(char* input, int length, char **argv, int *argc) {		

	if (length == 0) {

		*argc = 0;
		return;
	}

	int i = 0;
	int arg_c = 1;

	char *arg_s = input;
	char **args_v = argv;

	*args_v = arg_s;
	args_v++;

	do {

		if (*arg_s == ' ') {

			*arg_s = '\0';
			*args_v = (arg_s + 1); // argument starts at next character position

			arg_c++;
			args_v++;
		}

		arg_s++;

	} while((++i) < length);

	*argc = arg_c;
}

static int shell_command(void) {

	if (!cmd_length) return 0;

	if (strcmp(cmd_buffer, CODE) == 0) {

		_kernel_outStringFormat("%d\n", ret_code);
		return 0;
	}

	char *args_input = strchr(cmd_buffer, ' ');

	args_input = (args_input == NULL) ?
			(cmd_buffer + cmd_length) : (args_input + 1);

	uint8_t args_len = strlen(args_input);

	if (args_len > 0) {
		*(args_input - 1) = '\0';
	}

	// seek for internal command input

	for (command_t *cmd = (command_t *) &__cmd_table; 
			cmd != (command_t *) &__end_cmd_table; cmd++) {

		if ((cmd -> text != NULL) &&
				strcmp(cmd_buffer, cmd -> text) == 0) {

			int argc;
			char **argv = cmd_args;
						
			parse_cmdArgs(args_input, args_len, (++argv), &argc);

			return (cmd -> handler)((++argc), (const char **) cmd_args);
		}
	}

	_kernel_outString(BAD_COMMAND_MSG);
	return -1;
}

static void display_prompt(void) {

	char s_prompt[MAX_COMMAND_BUFFER + 1];

	_shell_prompt(s_prompt, MAX_COMMAND_BUFFER);
	_kernel_outString(s_prompt);
}

static void display_stackCommand(uint8_t direction) {

	char *cmd = cmd_buffer;
	if (!peak_stackCommand(cmd, direction)) { 
		return;
	}

	// clear cmd prompt

	for (size_t i = 0; i < cmd_length; i++) {
		_kernel_outChar(0x7f); // ascii del
	}

    cmd_length = strlen(cmd);
    _kernel_outString(cmd);
}
