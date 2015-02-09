#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>

#if defined(__i386__)
#include "../arch/i386/idt.h"
#include "../arch/i386/pic.h"
#include "../arch/i386/time.h"
#include "../arch/i386/pit.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/draw.h"
#include "../arch/i386/vfs.h"
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
	paging_initialize();
	sti(); //turn on interupts
	printf("Interupts On\n");
#endif

	printf("CometOS ver 0.0.0  -  ");
	printf("time:%i:%i:%i\n",time_get(2), time_get(1), time_get(0));
	printf("Hello, kernel World!\n\n");

	printf("Multiboot Data:\n");
	printf("MULTIBOOT header at 0x%x:\n", (uintptr_t)mboot_ptr);
	printf("Flags : 0x%x ", mboot_ptr->flags);
	printf("Mem Lo: 0x%x ", mboot_ptr->mem_lower);
	printf("Mem Hi: 0x%x ", mboot_ptr->mem_upper);
	printf("Boot d: 0x%x\n", mboot_ptr->boot_device);
	printf("cmdlin: 0x%x ", mboot_ptr->cmdline);
	printf("Mods : 0x%x ", mboot_ptr->mods_count);
	printf("Addr : 0x%x ", mboot_ptr->mods_addr);
	printf("Syms : 0x%x\n", mboot_ptr->num);
	printf("Syms : 0x%x ", mboot_ptr->size);
	printf("Syms : 0x%x ", mboot_ptr->addr);
	printf("Syms : 0x%x ", mboot_ptr->shndx);
	printf("MMap : 0x%x\n", mboot_ptr->mmap_length);
	printf("Addr : 0x%x ", mboot_ptr->mmap_addr);
	printf("Drives: 0x%x ", mboot_ptr->drives_length);
	printf("Addr : 0x%x ", mboot_ptr->drives_addr);
	printf("Config: 0x%x\n", mboot_ptr->config_table);
	printf("Loader: 0x%x ", mboot_ptr->boot_loader_name);
	printf("APM : 0x%x ", mboot_ptr->apm_table);
	printf("VBE Co: 0x%x ", mboot_ptr->vbe_control_info);
	printf("VBE Mo: 0x%x\n", mboot_ptr->vbe_mode_info);
	printf("VBE In: 0x%x ", mboot_ptr->vbe_mode);
	printf("VBE se: 0x%x ", mboot_ptr->vbe_interface_seg);
	printf("VBE of: 0x%x ", mboot_ptr->vbe_interface_off);
	printf("VBE le: 0x%x\n", mboot_ptr->vbe_interface_len);

	//printf("addr:%i", *get_page(0x10, 1, 0));

    for (;;); //main loop
}

void kernel_hang(void)
{
	printf("Kernel hlt");
}