#include <kernel/ui/log.h>
#include <kernel/portio.h>

volatile long timer_ticks = 0;
volatile unsigned long ticker = 0;

void pit_phase(int hz)
{
	int divisor = 1193180 / hz;
	outport8(0x43, 0x36);
	outport8(0x40, divisor & 0xFF);
	outport8(0x40, (divisor >> 8) & 0xFF);
}

void pit_handler(struct regs *r)
{
	++timer_ticks;
}

void sleep(int ticks)
{
	long eticks;
	eticks = (long)timer_ticks + (long)ticks;
	while(timer_ticks < eticks)
	{
		asm("hlt");
	}
}

void pit_install(void)
{
    irq_install_handler(0, pit_handler);
	pit_phase(100); /* 100Hz */

    log_print(NOTICE, "PIT");
}