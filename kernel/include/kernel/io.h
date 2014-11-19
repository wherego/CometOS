#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

unsigned char inportb(unsigned int port);
void outportb(unsigned int port,unsigned char value);
void stopints();

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};

#endif
