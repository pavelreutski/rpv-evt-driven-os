#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "events.h"

void _kernel_stdio(void);

void _kernel_outLn(void);
void _kernel_outTab(void);

void _kernel_outChar(const char code);
void _kernel_outString(const char* str);

void _kernel_outStringFormat(const char *fmt, ...);

void _kernel_stringFormat(char *s, char const* fmt, ...);
void _kernel_vstringFormat(char *s, char const* fmt, va_list args);

void _kernel_getKey(console_key_t *key);
bool _kernel_tryGetKey(console_key_t *key);