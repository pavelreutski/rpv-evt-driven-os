#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void _kernel_fio(void);

char _kernel_cdrive(void);

bool _kernel_unmount(void);
bool _kernel_mount(char letter);

size_t _kernel_drives(char *drive_lt, size_t nlt);