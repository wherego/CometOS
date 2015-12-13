#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);
__attribute__((__noreturn__))
void exit(int);

size_t lfind(const char * str1, const char str2);

#ifdef __cplusplus
}
#endif

#endif
