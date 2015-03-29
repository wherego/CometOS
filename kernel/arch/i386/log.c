#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if __STDC_HOSTED__ /* TODO: Hack until we have our own <stdint.h>. */
#include <stdint-gcc.h>
#else
#include <stdint.h>
#endif

#include <kernel/tty.h>
#include <kernel/vga.h>
#include "../arch/i386/log.h"
#include "../arch/i386/time.h"

static char * c_messages[] = {
	"NOTICE",
	"INFO",
	"WARNING",
	"ERROR",
	"CRITICAL"
};

static void print_dec(unsigned int value, unsigned int width, char * buf, int * ptr ) {
	unsigned int n_width = 1;
	unsigned int i = 9;
	while (value > i && i < UINT32_MAX) {
		n_width += 1;
		i *= 10;
		i += 9;
	}

	int printed = 0;
	while (n_width + printed < width) {
		buf[*ptr] = '0';
		*ptr += 1;
		printed += 1;
	}

	i = n_width;
	while (i > 0) {
		unsigned int n = value / 10;
		int r = value % 10;
		buf[*ptr + i - 1] = r + '0';
		i--;
		value = n;
	}
	*ptr += n_width;
}

/*
 * Hexadecimal to string
 */
static void print_hex(unsigned int value, unsigned int width, char * buf, int * ptr) {
	int i = width;

	if (i == 0) i = 8;

	unsigned int n_width = 1;
	unsigned int j = 0x0F;
	while (value > j && j < UINT32_MAX) {
		n_width += 1;
		j *= 0x10;
		j += 0x0F;
	}

	while (i > (int)n_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		i--;
	}

	i = (int)n_width;
	while (i-- > 0) {
		buf[*ptr] = "0123456789abcdef"[(value>>(i*4))&0xF];
		*ptr += + 1;
	}
}

vasprintf(char * buf, const char *fmt, va_list args) {
	int i = 0;
	char *s;
	int ptr = 0;
	int len = strlen(fmt);
	for ( ; i < len && fmt[i]; ++i) {
		if (fmt[i] != '%') {
			buf[ptr++] = fmt[i];
			continue;
		}
		++i;
		unsigned int arg_width = 0;
		while (fmt[i] >= '0' && fmt[i] <= '9') {
			arg_width *= 10;
			arg_width += fmt[i] - '0';
			++i;
		}
		/* fmt[i] == '%' */
		switch (fmt[i]) {
			case 's': /* String pointer -> String */
				s = (char *)va_arg(args, char *);
				if (s == NULL) {
					s = "(null)";
				}
				while (*s) {
					buf[ptr++] = *s++;
				}
				break;
			case 'c': /* Single character */
				buf[ptr++] = (char)va_arg(args, int);
				break;
			case 'x': /* Hexadecimal number */
				print_hex((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;
			case 'd': /* Decimal number */
				print_dec((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;
			case '%': /* Escape */
				buf[ptr++] = '%';
				break;
			default: /* Nothing at all, just dump it */
				buf[ptr++] = fmt[i];
				break;
		}
	}
	/* Ensure the buffer ends in a null */
	buf[ptr] = '\0';
	return ptr;

}

static char buffer[1024];

int log_print(enum debug_status status, const char* restrict format, ...)
{
	va_list args;
	va_start(args, format);
	vasprintf(buffer, format, args);
	va_end(args);

	char * debug_msg;
	debug_msg = "";
	debug_msg = c_messages[status];

	if (status == 0)
	{
		printf("[");
		terminal_setcolor(make_color(COLOR_GREEN, COLOR_BLACK));
		printf("OK");
		terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));
		printf("]");
		printf(" [%i.%i:%i:%d] %s\n", time_get(2), time_get(1), time_get(0), format, buffer);
	}
	else
	{
		printf("[%i.%i:%i:%d] %s\n", time_get(2), time_get(1), time_get(0), format, buffer);
	}
}