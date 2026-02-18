#pragma once

#include <stddef.h>
#include <stdbool.h>

void _kernel_jentry(char const* fmt, ...);
bool _kernel_jnxtentry(char *entry, const size_t max_len, size_t *const entry_len);
