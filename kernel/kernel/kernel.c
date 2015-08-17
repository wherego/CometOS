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
	printf("Hello, kernel World!");

#if DEBUG

	printf("DEBUG\n");

#endif

	/*uint32_t *ptr = (uint32_t*)0xA0000000;
	uint32_t do_page_fault = *ptr;
	printf(do_page_fault);*/

	array_t * array = array_create(0);

	array_node_t * anode = array_node_create(0x0, NULL);
	array_node_sort(anode, array);

	array_node_t * bnode = array_node_create(0x1, NULL);
	array_node_sort(bnode, array);

	array_node_t * cnode = array_node_create(0x3, NULL);
	array_node_sort(cnode, array);

	array_node_t * tnode = array_node_find(4, array);

	terminal_setcolor(make_color(COLOR_LIGHT_BLUE, COLOR_BLACK));
	printf("\nArray:%x | Order:%x | Start:%x | End:%x\n", array, array->order, array->start, array->end);
	terminal_setcolor(make_color(COLOR_LIGHT_RED, COLOR_BLACK));
	printf("NODE A:%x | index:%x | next:%x | prev:%x\n", anode, anode->index, anode->next, anode->prev);
	printf("NODE B:%x | index:%x | next:%x | prev:%x\n", bnode, bnode->index, bnode->next, bnode->prev);
	printf("NODE C:%x | index:%x | next:%x | prev:%x\n", cnode, cnode->index, cnode->next, cnode->prev);
	terminal_setcolor(make_color(COLOR_LIGHT_GREEN, COLOR_BLACK));
	printf("Find:%x | index:%x | next:%x | prev:%x\n", tnode, tnode->index, tnode->next, tnode->prev);
	terminal_setcolor(make_color(COLOR_BLUE, COLOR_BLACK));
	
	printf("DELETE ARRAY:%x", array_delete(array));
	terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));

    for (;;); //main loop
}

void kernel_hang(void)
{
	log_print(CRIT, "Kernel hlt");
}