#ifndef _STDIO_H
#define _STDIO_H 1
 
#include <sys/cdefs.h>
 
__BEGIN_DECLS
 
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
 
__END_DECLS
 
#endif
