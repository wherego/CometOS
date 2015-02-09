#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../arch/i386/paging.h"
#include <kernel/portio.h>

#define PAGESIZE_HEX 0x1000
#define PAGESIZE_DEC 4096
#define TABLESIZE 1024

uint32_t page_directory[TABLESIZE] __attribute__((aligned(PAGESIZE_DEC)));
uint32_t first_page_table[TABLESIZE] __attribute__((aligned(PAGESIZE_DEC)));

void paging_initialize(uint32_t mem_upper)
{

}

void map_page(void * physaddr, void * virtualaddr, unsigned int flags)
{
  // Make sure that both addresses are page-aligned. 
  unsigned long pdindex = (unsigned long)virtualaddr >> 22;
  unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
  unsigned long * pd = (unsigned long *)0xFFFFF000;
  // Checking whether the PD entry is present.
  // It is not present createing a new empty PT and adjusting the PDE
 
  unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
  // Here you need to check whether the PT entry is present.
  // When it is, then there is already a mapping present. What do you do now?
 
  pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01; // Present
 
  // Now you need to flush the entry in the TLB
  // or you might not notice the change.
}

void * get_physaddr(void * virtualaddr)
{
  unsigned long pdindex = (unsigned long)virtualaddr >> 22;
  unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
  unsigned long * pd = (unsigned long *)0xFFFFF000;
  // Here you need to check whether the PD entry is present.
 
  unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
  // Here you need to check whether the PT entry is present.
 
  return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}

/*int pd_checkentry(long table)
 {
  if(page_directory[table])
    return 1;
  else
    return 0;
 }

int pt_checkentry(long * table, long virtualaddr)
 {
  virtualaddr /= PAGESIZE_HEX;
  virtualaddr /= TABLESIZE;

  if(pd_checkentry(table) && table[virtualaddr])
    return 1;
  else
    return 0;
 }

 void paging_createtable(char * name)
 {
 // uint32_t name[TABLESIZE] __attribute__((aligned(PAGESIZE_DEC)));
 }*/

 void paging_installtable(long * table)
 {
  unsigned int i;

  for(i = 0; page_directory[i] != 0; i++)
  {}
//  page_directory[i] = ((unsigned int))table | 3;
 }