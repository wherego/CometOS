#include <stdint.h>
#include <string.h>

#include "idt.h"

#include <kernel/ui/log.h>

#define IDT_TYPE_INTERRUPT 0xE /* Interrupts disabled in eflags. */
#define IDT_TYPE_TRAP 0xF      /* Interrupts unaffected in eflags. */

#define IDT_FLAG_PRESENT (1 << 7)
#define IDT_FLAG_DPL_SHIFT 5
#define IDT_FLAG_DPL_BITS 2
#define IDT_FLAG_TYPE_SHIFT 0
#define IDT_FLAG_TYPE_BITS 4

struct idt_entry
{
	uint16_t handler_low;
	uint16_t selector;
	uint8_t ist;
	uint8_t flags;
	uint16_t handler_high;
};

struct idt_entry idt[256];

void lidt(uintptr_t base, size_t limit)
{
	asm volatile ("subl $6, %%esp\n\t"
	              "movw %w0, 0(%%esp)\n\t"
	              "movl %1, 2(%%esp)\n\t"
	              "lidt (%%esp)\n\t"
	              "addl $6, %%esp" : : "rN"(limit), "r"(base));
}

void idt_format_normal_entry(struct idt_entry* entry,
                             void (*handler)(void),
                             uint8_t type,
                             uint8_t rpl)
{
	entry->handler_low = (uintptr_t) handler >> 0 & 0xFFFF;
	entry->selector = 0x08 /* Kernel Code Segment */;
	entry->ist = 0;
	entry->flags = IDT_FLAG_PRESENT |
	               rpl << IDT_FLAG_DPL_SHIFT |
	               type << IDT_FLAG_TYPE_SHIFT;
	entry->handler_high = (uintptr_t) handler >> 16 & 0xFFFF;
}

void idt_initialize()
{
	memset(&idt, 0, sizeof(idt));
	idt_format_normal_entry(&idt[0], isr0, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[1], isr1, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[2], isr2, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[3], isr3, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[4], isr4, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[5], isr5, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[6], isr6, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[7], isr7, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[8], isr8, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[9], isr9, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[10], isr10, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[11], isr11, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[12], isr12, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[13], isr13, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[14], isr14, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[15], isr15, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[16], isr16, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[17], isr17, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[18], isr18, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[19], isr19, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[20], isr20, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[21], isr21, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[22], isr22, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[23], isr23, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[24], isr24, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[25], isr25, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[26], isr26, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[27], isr27, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[28], isr28, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[29], isr29, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[30], isr30, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[31], isr31, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[32], irq0, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[32], irq0, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[33], irq1, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[34], irq2, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[35], irq3, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[36], irq4, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[37], irq5, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[38], irq6, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[39], irq7, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[40], irq8, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[41], irq9, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[42], irq10, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[43], irq11, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[44], irq12, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[45], irq13, IDT_TYPE_INTERRUPT, 0x8E);
	idt_format_normal_entry(&idt[46], irq14, IDT_TYPE_INTERRUPT, 0x0);
	idt_format_normal_entry(&idt[47], irq15, IDT_TYPE_INTERRUPT, 0x0);
	lidt((uintptr_t) idt, sizeof(idt) - 1);

	log_print(NOTICE, "IDT");
}
