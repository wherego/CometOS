#include <stdint.h>

#include "../pic.h"

#include <kernel/ui/log.h>

struct interrupt_context
{
	uint32_t cr2;
	uint32_t gs;
	uint32_t fs;
	uint32_t ds;
	uint32_t es;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t int_no;
	uint32_t err_code;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp; /* If (cs & 0x3) != 0 */
	uint32_t ss;  /* If (cs & 0x3) != 0 */
};



void *irq_routines[16];
void *isr_routines[32];

unsigned char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void irq_install_handler(int irq, void (*handler)(struct interrupt_context* int_ctx))
{
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

void isr_install_handler(int isr, void (*handler)(struct interrupt_context* int_ctx))
{
    isr_routines[isr] = handler;
}

void isr_uninstall_handler(int isr)
{
    isr_routines[isr] = 0;
}

void isr_handler(struct interrupt_context* int_ctx)
{
	//run irq_routines
	void (*handler)(struct interrupt_context* int_ctx);
	handler = isr_routines[int_ctx->int_no];
	if (handler)
		{
			handler(int_ctx->err_code);
		}

    else if (int_ctx->int_no < 32)
    {
    	printf(exception_messages[int_ctx->int_no]);
    	printf("\nFult number:%i\n\n", int_ctx->int_no);
        printf("System Halted!\n");
    }

	(void) int_ctx;
	asm("hlt");
}

void irq_handler(struct interrupt_context* int_ctx)
{
	uint8_t irq = int_ctx->int_no - 32;

	// Handle the potentially spurious interrupts IRQ 7 and IRQ 15.
	if ( irq == 7 && !(pic_read_isr() & (1 << 7)) )
		return;
	if ( irq == 15 && !(pic_read_isr() & (1 << 15)) )
		return pic_eoi_master();

	//run irq_routines
	void (*handler)(struct interrupt_context* int_ctx);
	handler = irq_routines[int_ctx->int_no - 32];
	if (handler)
		{
			handler(int_ctx);
		}

	(void) int_ctx;

	// Send the end of interrupt signal to the relevant PICs.
	if ( 8 <= irq )
		pic_eoi_slave();
	pic_eoi_master();
}

void interrupt_handler(struct interrupt_context* int_ctx)
{
	if ( int_ctx->int_no < 32 )
		isr_handler(int_ctx);
	else if ( 32 <= int_ctx->int_no && int_ctx->int_no < 32 + 16 )
		irq_handler(int_ctx);
}