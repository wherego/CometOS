#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>

#if defined(__i386__)
#include "../arch/i386/idt.h"
#include "../arch/i386/pic.h"
#include "../arch/i386/time.h"
#include "../arch/i386/pit.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/draw.h"
#include <kernel/portio.h>
#endif

void kernel_early(void)
{
	terminal_initialize();
	printf("CometOS ver 0.0.0\n");

}

void kernel_main(void)
{

#if defined(__i386__)
	idt_initialize();
	pic_initialize();
	keyboard_install();
	pit_install();
	sti(); //turn on interupts
	printf("Interupts On\n");
#endif

	printf("Hello, kernel World!\n");

    for (;;); //main loop
}

void kernel_hang(void)
{
	printf("Kernel hlt");
}
