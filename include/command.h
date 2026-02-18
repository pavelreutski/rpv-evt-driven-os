#pragma once

#include <stdint.h>

struct command_s {

    const char* text;
    int (*const handler)(const int argc, const char **argv);
};

typedef struct command_s command_t;

#define _SHELL_COMMAND(name, callback) \
    __attribute__((used, section(".cmd_table"))) \
    static const command_t name = { #name, callback }
