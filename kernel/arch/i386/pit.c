int timer_ticks = 0;

void pit_handler(struct regs *r)
{
    /* Increment our 'tick count' */
    timer_ticks++;
}

void sleep(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}

void pit_install(void)
{
    irq_install_handler(0, pit_handler);
}