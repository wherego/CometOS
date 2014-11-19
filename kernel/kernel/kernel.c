#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
 
#include <kernel/tty.h>
#include <kernel/io.h>
#include <kernel/gdt.h>
#include <kernel/irq.h>
#include <kernel/kb.h>
#include <kernel/isrs.h>
 
void kernel_early()
{
	gdt_initialize();
	terminal_initialize();
	init_idt();
}

void kernel_main()
{
	terminal_writestring("Hello, Kernel World");
}
