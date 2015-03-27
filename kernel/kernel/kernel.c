#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/vga.h>

#if defined(__i386__)
#include "../arch/i386/idt.h"
#include "../arch/i386/pic.h"
#include "../arch/i386/time.h"
#include "../arch/i386/pit.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/draw.h"
#include "../arch/i386/vfs.h"
#include "../arch/i386/log.h"
#include <kernel/portio.h>
#endif

#define NULL ((void *)0UL)

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(struct multiboot *mboot_ptr)
{

#if defined(__i386__)
	idt_initialize();
	pic_initialize();
	keyboard_install();
	pit_install();
	paging_initialize(mboot_ptr->mem_upper);
	sti(); //turn on interupts
	log_print(INFO, "Interupts On\n");
#endif

	multiboot_print(mboot_ptr);
	printf("--------------------------------------------------\n");
	printf("CometOS ver 0.0.0  -  time:%i:%i:%i\n",time_get(2), time_get(1), time_get(0));
	printf("Hello, kernel World!\n\n");

	//uint32_t *ptr = (uint32_t*)0xA0000000;
	//uint32_t do_page_fault = *ptr;
	//printf(do_page_fault);   

    for (;;); //main loop
}

void kernel_hang(void)
{
	log_print(CRIT, "Kernel hlt");
}