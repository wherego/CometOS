#include <stdint.h>
#include <string.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include <kernel/portio.h>

#define INDEX_GET(i) (i / PAGE_SIZE_HEX)
#define ADDR_GET(i) (i * PAGE_SIZE_HEX)

extern void *end_kernel;
uintptr_t kernel_pointer = (uintptr_t)&end_kernel;

struct bitmap frame_bitmap;

const uint32_t PAGE_SIZE_HEX = 0x1000;
const int PAGE_SIZE_DEC = 4096;
const uint32_t ENTRY_SIZE_HEX = 0x400;
const int ENTRY_SIZE_DEC = 1024;
const uint32_t ADDRESS_SIZE_HEX = 0x20;
const int ADDRESS_SIZE_DEC = 32;
const uint32_t USED = 0x1;
const uint32_t FREE = 0x0;

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

void frame_set(void * addr)
{
  uint32_t index = INDEX_GET((uint32_t)addr);
  frame_bitmap.entries[index] = USED;
}

void frame_free(void * addr)
{
  uint32_t index = INDEX_GET((uint32_t)addr);
  frame_bitmap.entries[index] = FREE;
}

int frame_test(void * addr)
{
  uint32_t index = INDEX_GET((uint32_t)addr);
  return frame_bitmap.entries[index];
}

void * frame_find()
{
  uint32_t i = 0;
  while(frame_bitmap.entries[i] != FREE)
  {
    if(ADDR_GET(i) == frame_bitmap.number)
    {
      return -1;
    }

    i++;
  }

  return (void *)ADDR_GET(i);
}

void * frame_alloc()
{
  uint32_t * frame = frame_find();
  if(frame == 0xffffffff)
  {
    return NULL;
  }

  frame_set(frame);
  return frame;
}

void * ponter_move_int(uint32_t size, uint32_t * physaddr, int align)
{
  if(align == 1 && (kernel_pointer & 0xFFFFF000))
  {
    kernel_pointer &= 0xFFFFF000;
    kernel_pointer += PAGE_SIZE_HEX;
  }

  if(physaddr)
  {
    *physaddr = kernel_pointer;
  }

  uint32_t i = kernel_pointer;
  kernel_pointer += size;

  return (void *)i;
}

void page_fault(uint32_t int_ctx)
{
  uint32_t addr;
  asm volatile("mov %%cr2, %0" : "=r" (addr));

  int present = !(int_ctx & 0x1);
  int rw = int_ctx & 0x2;
  int us = int_ctx & 0x4;
  int reserved = int_ctx & 0x8;
  int id = int_ctx & 0x10;

  printf("\n|---------------- Page Fault ----------------|\n");

  printf("Address: 0x%x\n", addr);
  printf("Present: %i\n", present);
  printf("RW: %i\n", rw);
  printf("US: %i\n", us);
  printf("Reserved: %i\n", reserved);
  printf("ID: %i\n", id);

  printf("|--------------------------------------------|\n");
}

void paging_initialize(uint32_t mem_lower, uint32_t mem_upper)
{
  frame_bitmap.number = (uint32_t)mem_upper / 4;
  frame_bitmap.entries = ponter_move_int((frame_bitmap.number / ADDRESS_SIZE_HEX), 0, 1);
  memset(frame_bitmap.entries, 0, frame_bitmap.number / ADDRESS_SIZE_HEX);

  //Map phy addr's for kernel
  uint32_t i = 0;
  while(i < kernel_pointer)
  {
    frame_set(i);
    i += PAGE_SIZE_HEX;
  }

  //Create and load kernel's paging directory
  uint32_t * kernel_directory = directory_create(0x00000002);
  loadPageDirectory(kernel_directory);

  //Map virtual addr's for kernel
  i = 0;
  while(i < kernel_pointer)
  {
    page_map(i, i, 3);
    i += PAGE_SIZE_HEX;
  }

  isr_install_handler(14, page_fault);

  //Enable paging
  enablePaging();
  log_print(NOTICE, "Paging");
}

void * table_create(unsigned int flags)
{
  uint32_t * table = frame_alloc();

  if(table == NULL)
  {
    return NULL;
  }
  
  int i;
  for(i = 0; i < ENTRY_SIZE_DEC; i++)
    {
      table[i] = (i * PAGE_SIZE_HEX) | flags;
    }

  return table;
}

void * table_map(void * directory, unsigned long pdindex, void * table_addr, unsigned int flags)
{
  unsigned long * pd = (unsigned long *)directory;
  pd[pdindex] = ((unsigned long)table_addr) | flags;
}

void table_unmap(void * directory, unsigned long pdindex)
{
  unsigned long * pd = (unsigned long *)directory;
  pd[pdindex] = 0x00000000;
}

void table_remove(void * table_addr)
{
  frame_free(table_addr);
}

void * directory_get(void)
{
  uint32_t cr3;
  asm volatile("mov %%cr3, %0": "=r"(cr3));

  return (void *)cr3;
}

void * directory_create(uint32_t flags)
{
  uint32_t * directory = (uint32_t)frame_alloc();

  if(directory == NULL)
  {
    return NULL;
  }

  int i;
  for(i = 0; i < ENTRY_SIZE_DEC; i++)
  {
    directory[i] = flags;
  }

  return directory;
}

void directory_remove(void * directory_addr)
{
  uint32_t dir = directory_get();
  if(dir == directory_addr)
  {
    return NULL;
  }

  frame_free(directory_addr);
}

void * page_map(void * physaddr, void * virtualaddr, unsigned int flags)
{
  if (((unsigned int)virtualaddr & 0xFFF) || ((unsigned int)physaddr & 0xFFF))
  {
    return NULL;
  }

  unsigned long page_index = (unsigned long)virtualaddr / PAGE_SIZE_HEX;
  unsigned long pdindex = (unsigned long)page_index / ENTRY_SIZE_HEX;
  unsigned long ptindex = (unsigned long)page_index % ENTRY_SIZE_HEX;

  unsigned long * pd = (unsigned long *)directory_get(); 
  // Seek out or create Pagetable
  if ((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    uint32_t * page_table = table_create(3);
    table_map(pd, pdindex, page_table, 3);
  }

  unsigned long * pt = ((unsigned long *)pd) + (ENTRY_SIZE_HEX * pdindex);
  if ((unsigned int)pt[ptindex] & USED)
  {
    return NULL;
  }

  pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | USED;
  flush_tlb(virtualaddr);

  return (void *)physaddr;
}

int page_unmap(void * virtualaddr)
{
  if ((unsigned int)virtualaddr & 0xFFF)
  {
    return 0;
  }

  unsigned long page_index = (unsigned long)virtualaddr / PAGE_SIZE_HEX;
  unsigned long pdindex = (unsigned long)page_index / ENTRY_SIZE_HEX;
  unsigned long ptindex = (unsigned long)page_index % ENTRY_SIZE_HEX;

  unsigned long * pd = (unsigned long *)directory_get();

  if ((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    return 1;
  }

  unsigned long * pt = ((unsigned long *)pd) + (ENTRY_SIZE_HEX * pdindex);
  if ((((unsigned int)pt[ptindex]) & USED) != USED)
  {
    return 1;
  }

  pt[ptindex] = 0x00000000;
  flush_tlb(virtualaddr);

  return 1;
}

void * page_physaddr(void * virtualaddr)
{
  unsigned long page_index = (unsigned long)virtualaddr / PAGE_SIZE_HEX;
  unsigned long pdindex = (unsigned long)page_index / ENTRY_SIZE_HEX;
  unsigned long ptindex = (unsigned long)page_index % ENTRY_SIZE_HEX;
 
  unsigned long * pd = (unsigned long *)directory_get();

  //Check whether the PD entry is present.
  if((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    return NULL;
  }
 
 unsigned long * pt = ((unsigned long *)pd) + (ENTRY_SIZE_HEX * pdindex);

  //Check whether the PT entry is present.
  if((((unsigned int)pt[ptindex]) & USED) != USED)
  {
    return NULL;
  }
 
  return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}