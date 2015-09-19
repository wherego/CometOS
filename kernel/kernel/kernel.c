#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/portio.h>

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
#include "../arch/i386/array.h"
#include "../arch/i386/liballoc.h"
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
	#ifdef DEBUG
		log_print(INFO, "Interupts On");
	#endif

	floppy_initialize(6);
	floppy_drive_set(0);
#endif

	#ifdef DEBUG
		multiboot_print(mboot_ptr);
		printf("--------------------------------------------------\n");
	#endif

	printf("CometOS ver 0.0.0  -  time:%i:%i:%i\n",time_get(2), time_get(1), time_get(0));
	printf("Hello, kernel World!\n");

	uint8_t * sector = floppy_read(0);
	printf("Sector:%x\n", sector);

	int i = 0;
	for(int c = 0; c < 4; c++)
	{
		for(int j = 0; j < 128; j++)
		{
			printf("%x", sector[i + j]);
			i += 128;
		}
	}

    for (;;); //main loop
}

void kernel_hang(void)
{
	log_print(CRIT, "Kernel hlt");
}