#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/fs/initrd.h>
#include <kernel/fs/vfs.h>
#include <kernel/ui/log.h>
#include <kernel/multiboot.h>
#include <kernel/portio.h>
#include <kernel/kb.h>

#if defined(__i386__)
#include "../arch/i386/irq/idt.h"
#include "../arch/i386/mem/paging.h"
#include "../arch/i386/mem/heap.h"
#include "../arch/i386/mem/liballoc.h"
#include "../arch/i386/gui/draw.h"
#include "../arch/i386/task/task.h"
#include "../arch/i386/pic.h"
#include "../arch/i386/time.h"
#include "../arch/i386/pit.h"
#endif

uint32_t initial_esp;

void kernel_early(void)
{
	terminal_initialize();
}

void kernel_main(struct multiboot *mboot_ptr, uint32_t initial_stack)
{

#if defined(__i386__)
	#ifdef DEBUG
		multiboot_print(mboot_ptr); //TODO: make a multiboot function.
		printf("--------------------------------------------------\n");
	#endif

	idt_initialize();
	pic_initialize();
	pit_install();
	paging_initialize(mboot_ptr->mem_lower, mboot_ptr->mem_upper);

	sti(); //turn on interupts
	#ifdef DEBUG
		log_print(INFO, "Interupts On");
	#endif

	floppy_initialize(6);
	floppy_drive_set(0);
#endif

	multiboot_print(mboot_ptr);
	keyboard_install();
	extern uint32_t module_start;
	initrd_initialize(module_start);

	printf("CometOS ver 0.0.0  -  time:%i:%i:%i\n",time_get(2), time_get(1), time_get(0));
	printf("Hello, kernel World!\n");

    for (;;); //main loop
}

void kernel_hang(void)
{
	log_print(CRIT, "Kernel hlt");
}