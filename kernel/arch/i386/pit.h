#ifndef _MYOS_ARCH_I386_PIT_H
#define _MYOS_ARCH_I386_PIT_H

void pit_handler(void);
void Sleep(uint32_t delay);
void pit_install(void);
void pit_phase(int hz);

#endif