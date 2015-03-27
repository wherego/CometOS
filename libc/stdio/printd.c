#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if __STDC_HOSTED__ /* TODO: Hack until we have our own <stdint.h>. */
#include <stdint-gcc.h>
#else
#include <stdint.h>
#endif

static char * c_messages[] = {
	"NOTICE",
	"INFO",
	"WARNING",
	"ERROR",
	"CRITICAL"
};

static char buffer[1024];

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

int printd(enum debug_status status, int time_h, int time_m, int time_s, const char* restrict format, ...)
{
	va_list args;
	va_start(args, format);
	vasprintf(buffer, format, args);
	va_end(args);

	char * debug_msg;
	debug_msg = c_messages[status];

	if (status = 0)
	{
		//[ok]
	}

	printf("[%i.%i:%i:%d]%s %s\n", time_h, time_m, time_s, format, debug_msg, buffer);
	return 1;
}