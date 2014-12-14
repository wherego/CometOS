#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>

#if defined(__i386__)
#include "../arch/i386/idt.h"
#include "../arch/i386/pic.h"
#include "../arch/i386/time.h"
#include <kernel/portio.h>
#endif

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(void)
{
	printf("Hello, kernel World!\n");

	unsigned int i = hex2dec(0x80);
	terminal_writestring(i);

#if defined(__i386__)
	idt_initialize();
	pic_initialize();
	keyboard_install();
	sti(); //turn on interupts
#endif

    for (;;); //loop
}
