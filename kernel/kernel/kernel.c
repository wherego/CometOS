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
#include "../arch/i386/heap.h"
#include "../arch/i386/task.h"
#include <kernel/portio.h>
#endif

uint32_t initial_esp;

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(struct multiboot *mboot_ptr, uint32_t initial_stack)
{

#if defined(__i386__)
	idt_initialize();
	pic_initialize();
	keyboard_install();
	pit_install();
	paging_initialize(mboot_ptr->mem_lower, mboot_ptr->mem_upper);
	sti(); //turn on interupts
	floppy_initialize(38);
	floppy_set_dma(0x8000);
	log_print(INFO, "Interupts On\n");
#endif

	//#ifdef DEBUG
	multiboot_print(mboot_ptr);
	printf("--------------------------------------------------\n");
	//#endif

	printf("CometOS ver 0.0.0  -  time:%i:%i:%i\n",time_get(2), time_get(1), time_get(0));
	printf("Hello, kernel World!\n");

	/*uint32_t *ptr = (uint32_t*)0xA0000000;
	uint32_t do_page_fault = *ptr;
	printf(do_page_fault);*/

	asm("movb $0x13, %ah");
	asm("int 0x10");
	asm("ret");

    for (;;); //main loop
}

void kernel_hang(void)
{
	log_print(CRIT, "Kernel hlt");
}