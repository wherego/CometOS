#ifndef _COMETOS_ARCH_I386_LOG_H
#define _COMETOS_ARCH_I386_LOG_H

enum debug_status
{
	NOTICE = 0,
	INFO = 1,
	WARNING = 2,
	ERROR = 3,
	CRIT = 4,
};

int log_print(enum debug_status status, const char* restrict format, ...);

#endif