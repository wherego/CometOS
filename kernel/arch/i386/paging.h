#ifndef _MYOS_ARCH_I386_PAGING_H
#define _MYOS_ARCH_I386_PAGING_H

typedef struct pageflag
{
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} pageflag_t;

int pd_checkentry(long table);
int pt_checkentry(long * table, long virtualaddr);

void paging_createtable(char name);
void paging_installtable(long * table);

#endif