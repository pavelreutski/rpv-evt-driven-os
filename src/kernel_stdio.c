#include <string.h>
#include <stdarg.h>

#include "events.h"
#include "console.h"

#include "kernel.h"
#include "kernel_stdio.h"
#include "kernel_signal.h"

static console_key_t key_Buffer;

static void onKernel_userConKey(evt_data_t const* evtData);

static void out_console(void **ctx, char const* s);
static void out_strbuffer(void **ctx, char const* s);

static void format_width(char **s, size_t len, size_t width, char pad);

static void format_int(void (*out)(void **, char const*), void **ctx, int value, size_t width, char pad);
static void format_hex(void (*out)(void **, char const*), void **ctx, unsigned value, size_t width, char pad);
static void format_char(void (*out)(void **, char const*), void **ctx, const int value, size_t width, char pad);

static void format_core(void (*out)(void **, char const*), void **ctx, char const* fmt, va_list args);

void _kernel_stdio(void) {

	set_con();
	_kernel_subEvt(EVT_USERCON_KEY, onKernel_userConKey);
}

void _kernel_outLn(void) {
	con_ln();
}

void _kernel_outTab(void) {
	con_tab();
}

void _kernel_outChar(const char code) {
	con_char(code);
}

void _kernel_outString(const char* str) {
	con_string(str);
}

void _kernel_stringFormat(char *s, char const* fmt, ...) {

	va_list args;
	va_start(args, fmt);

	format_core(out_strbuffer, (void **) &s, fmt, args);

	va_end(args);
}

void _kernel_vstringFormat(char *s, char const* fmt, va_list args) {
	format_core(out_strbuffer, (void **) &s, fmt, args);
}

void _kernel_outStringFormat(const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	format_core(out_console, (void **) NULL, fmt, args);

	va_end(args);
}

bool _kernel_tryGetKey(console_key_t *key) {

	memset(key, 0, sizeof(console_key_t));

	sigset_t set;

	_kernel_sigemptyset(&set);
	_kernel_sigaddset(&set, SIGIO);

	_kernel_sigprocmask(SIG_BLOCK, &set, NULL);

	sigset_t pending;
	_kernel_sigemptyset(&pending);

	_kernel_sigpending(&pending);
	
	bool is_sigInt = _kernel_sigismember(&pending, SIGIO);

	if (is_sigInt) {

		_kernel_sigprocmask(SIG_UNBLOCK, &set, NULL);
		memcpy(key, &key_Buffer, sizeof(console_key_t));
	}

	return is_sigInt;
}

void _kernel_getKey(console_key_t *key) {

	sigset_t set;

	_kernel_sigemptyset(&set);
	_kernel_sigaddset(&set, SIGIO);

	_kernel_sigprocmask(SIG_BLOCK, &set, NULL);

	int sig;
	_kernel_sigwait(&set, &sig);

	memcpy(key, &key_Buffer, sizeof(console_key_t));
}

static __attribute__((noinline)) void onKernel_userConKey(evt_data_t const* evtData) {
	
	_kernel_raise(SIGIO);
    memcpy(&key_Buffer, (console_key_t *) evtData, sizeof(console_key_t));
}

static void out_console(void **ctx, char const* s) {

	(void) ctx;
	con_string(s);
}

static void out_strbuffer(void **ctx, char const* s) {

	const char *str = s;
	char *buffer = *((char **) ctx);

	while (*str) {
		*(buffer++) = *(str++);
	}

	*buffer = '\0';
	*((char **) ctx) = buffer;
}

static void format_width(char **s, size_t len, size_t width, char pad) {

	while (len < width) {
		*(--(*s)) = pad;
		len++;
	}
}

static void format_int(void (*out)(void **, char const*), 
							void **ctx, int value, size_t width, char pad) {

	char str_buf[256];
	char *s = str_buf + sizeof(str_buf);

	*(--s) = '\0';
	
	size_t len = 0;
	unsigned d = (value < 0) ? (~((unsigned) value) + 1) : (unsigned) value;

	do {
		*(--s) = '0' + (d % 10);		
		d /= 10;
		len++;
	} while(d > 0);

	if (value < 0) {

		len++;
		*(--s) = '-';
	}

	format_width(&s, len, width, pad);
	
	out(ctx, s);
}

static void format_hex(void (*out)(void **, char const*), 
							void **ctx, unsigned value, size_t width, char pad) {
	
	size_t s_width = 
		width < (sizeof(unsigned) * 2) ? 
				sizeof(unsigned) * 2 : width;

 	char s_buff[s_width + 1];
	static const char hexdec[] = "0123456789ABCDEF";

	char *s = s_buff + sizeof(s_buff);

	*(--s) = '\0';
	
	size_t len = 0;
	unsigned d = value;	

	do {
		*(--s) = hexdec[(d & 0x0F)];		
		d >>= 4;
		len++;
	} while (d > 0);

	format_width(&s, len, width, pad);
	
	out(ctx, s);
}

static void format_char(void (*out)(void **, char const*), 
							void **ctx, const int value, size_t width, char pad) {

	size_t len = 1;
	char s[] = { pad, '\0' };

	while ((len++) < width) {
		out(ctx, s);
	}
	
	*s = (char) value;
	out(ctx, s);
}

static void format_core(void (*out)(void **, char const*), void **ctx, char const* fmt, va_list args) {

	while(*fmt) {		

		if (*fmt != '%') {

			char literal[] = { *fmt++, '\0' };
			out(ctx, literal);
			continue;
		}

		fmt++;
		
		char pad = ' ';
		if (*fmt == '0') {
			pad = '0';
			fmt++;
		}
		
		size_t width = 0;
		while(*fmt >= '0' && *fmt <= '9') {

			uint8_t digit = (*fmt - '0');

			width *= 10;
			width += digit;
			
			fmt++;
		}

		char spec = *(fmt++);

		switch(spec) {

			case '\0':  {
				return;
			} break;

			case 's':  { 
				char const* s = va_arg(args, char const*);			
				out(ctx, s);
			} break;

			case 'd': {

				const int value = va_arg(args, const int);
				format_int(out, ctx, value, width, pad);
			} break;

			case 'x': {

				const unsigned value = va_arg(args, const unsigned);
				format_hex(out, ctx, value, width, pad);
			} break;

			case 'c': {

				const int value = va_arg(args, const int);
				format_char(out, ctx, value, width, pad);
			} break;

			default: { 

				char literal[] = { spec, '\0' };
				out(ctx, literal); 
			} break;
		}
	}
}
