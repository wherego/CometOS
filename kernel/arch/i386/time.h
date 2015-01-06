#ifndef _MYOS_ARCH_I386_TIME_H
#define _MYOS_ARCH_I386_TIME_H

char time_get(char type);
unsigned int get_update_in_progress_flag();
unsigned int get_RTC_register(int reg);

#endif
