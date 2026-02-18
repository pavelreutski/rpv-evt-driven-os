#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern size_t MAX_X;
extern size_t MAX_Y;

void con_ln(void);
void con_tab(void);

void con_clear(void);

void con_char(const char code);
void con_string(const char* str);

void con_xy(size_t cx, size_t cy);

size_t con_getx(void);
size_t con_gety(void);

void set_con(void);