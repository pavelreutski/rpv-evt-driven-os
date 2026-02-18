#include <string.h>

#include "monitor.h"
#include "console.h"

#include "kernel_stdio.h"

size_t text_monitor(char const* s) {	

	char c;
	size_t t_len;

	char *text;

	for (text = (char *) s, c = *text, t_len = 0; 
			c != '\0'; text++, t_len++, c = *text) {

		_kernel_outChar(c);

		if ((*(text + 1) != '\0') && 
				(con_getx() == 0) && (con_gety() == (MAX_Y - 1))) {

			console_key_t key;

			_kernel_outString("--More--");
			_kernel_getKey(&key);

			_kernel_outChar('\n');
		}
	}

	return t_len;
}

size_t hex_monitor(void const* mem, const size_t s) {

    uint8_t *blck_data = (uint8_t *) mem;

	#define LINE_BYTEW      3
	#define LINE_BYTES      16

	for (size_t offset = 0; offset < s; offset += LINE_BYTES) {
		
		if (con_gety() == (MAX_Y - 1)) {

			console_key_t key;

			_kernel_outString("--More--");
			_kernel_getKey(&key);

			_kernel_outChar('\n');

			if (key.scan_code == 0x76) { // esc key
				return offset;
			}
		}

		_kernel_outStringFormat("%04x: ", offset);

		for (size_t i = 0; i < LINE_BYTES; i++) {

			if ((offset + i) >= s) {
				_kernel_outStringFormat("%3c", ' ');								
			} else {
				_kernel_outStringFormat("%02x ", blck_data[offset + i]);
			}
		}

		for (size_t i = 0; (offset + i) < s && i < LINE_BYTES; i++) {

			char c = blck_data[offset + i];

			bool is_num = (c >= '0') && (c <= '9');
			bool is_char = ((c >= 'a') && (c <= 'z')) || 
								((c >= 'A') && (c <= 'Z'));

			c = (is_num || is_char) ? c : '.'; 

			_kernel_outChar(c);
		}

		_kernel_outChar('\n');		
	}

    return s;
}