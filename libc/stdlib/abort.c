#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort(void)
{
#if __STDC_HOSTED__
	// TODO: Properly implement abort().
	exit(1);
#elif defined(__is_myos_kernel)
	// TODO: Add proper kernel panic.
	//printf("\n\n*** KERNEL PANIC ***\nError:%s\nTime:%i:%i:%i\nFile:%s\nLine:%i\n********************\n\n", error_message, time_get(2), time_get(1), time_get(0), file, line);
	while ( 1 ) { }
#else
#error "You need to implement abort() in this freestanding environment."
#endif
	__builtin_unreachable();
}