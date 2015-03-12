#ifndef _COMETOS_ARCH_I386_PIT_H
#define _COMETOS_ARCH_I386_PIT_H

void pit_handler(void);
void Sleep(uint32_t delay);
void pit_install(void);
void pit_phase(int hz);

#endif
