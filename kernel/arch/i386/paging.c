#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include "../arch/i386/array.h"

#include <kernel/portio.h>

#define INDEX_GET(i) (i / PAGE_SIZE_HEX)
#define ADDR_GET(i) (i * PAGE_SIZE_HEX)

#define DIR_GET(i) (((i) >> 22) & 0x3ff)
#define TLB_GET(i) (((i) >> 12) & 0x3ff)
#define PHY_GET(i) (i & ~0xfff)

extern void *end_kernel;
uintptr_t kernel_pointer = (uintptr_t)&end_kernel;

bitmap_t * frame_bitmap;
heap_t * kernel_heap;
int paging_state = 0;

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
  bitmap_entrie_set(index, USED, frame_bitmap);
}

void frame_free(void * addr)
{
  uint32_t index = INDEX_GET((uint32_t)addr);
  bitmap_entrie_set(index, FREE, frame_bitmap);
}

int frame_test(void * addr)
{
  uint32_t index = INDEX_GET((uint32_t)addr);
  return bitmap_entrie_check(index, frame_bitmap);
}

void * frame_find()
{
  return (void *)ADDR_GET(bitmap_entrie_find(0x1, 0, frame_bitmap));
}

void * frame_alloc()
{
  uint32_t * frame = frame_find();
  if(frame == 0xffffffff)
  {
    return NULL;
  }

  frame_set(frame);
  return (void *)frame;
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

  printf("\n\n|---------------- ");
  terminal_setcolor(make_color(COLOR_RED, COLOR_BLACK));
  printf("Page Fault");
  terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));
  printf(" ----------------|\n");

  terminal_setcolor(make_color(COLOR_LIGHT_BLUE, COLOR_BLACK));

  printf("Address: 0x%x\n", addr);
  printf("Present: %i\n", present);
  printf("RW: %i\n", rw);
  printf("US: %i\n", us);
  printf("Reserved: %i\n", reserved);
  printf("ID: %i\n", id);

  terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_BLACK));
  printf("|--------------------------------------------|");
}

void paging_initialize(uint32_t mem_lower, uint32_t mem_upper)
{
  frame_bitmap = bitmap_create((uint32_t)mem_upper / 4);
  kernel_heap = heap_create(mem_upper, 0, 1, 0);
  //bitmap_map(0, INDEX_GET(kernel_pointer), 0x1, kernel_heap->bitmap);
  log_print(NOTICE, "Kernel Heap");

  //Map phy addr's and heap for kernel
  uint32_t i = 0;
  while(i < kernel_pointer)
  {
    frame_set(i);
    i += PAGE_SIZE_HEX;
  }

  //Create and load kernel's paging directory
  uint32_t * kernel_directory = directory_create(0x00000002);
  directory_load(kernel_directory);

  //Map virtual addr's for kernel
  i = 0;
  while(i < kernel_pointer)
  {
    page_map(i, i, 3);
    i += PAGE_SIZE_HEX;
  }

  isr_install_handler(14, page_fault);

  //Enable paging
  paging_enable();
  paging_state = 1;
  log_print(NOTICE, "Paging");
}

void * table_create(unsigned int flags)
{
  uint32_t * table = frame_alloc();
  if(table == NULL)
  {
    return NULL;
  }

  if(paging_state == 0)
  {
    int i;
    for(i = 0; i < ENTRY_SIZE_DEC; i++)
    {
      table[i] = (i * PAGE_SIZE_HEX) | 3;
    }
  }
  else
  {
    memset(table, 0, table + PAGE_SIZE_HEX);

    if(flags)
    {
      int i;
      for(i = 0; i < ENTRY_SIZE_DEC; i++)
      {
        table[i] |= flags;
      }
    }
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

void * table_copy(void * table, void * copy)
{
  uint32_t * start = (uint32_t)table;
  uint32_t * end = (uint32_t)copy;

  int i;
  for(i = 0; i < ENTRY_SIZE_DEC; i++)
    {
      end[i] = start[i];
    }

    return (void *)copy;
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

void * directory_copy(void * directory, void * copy)
{
  uint32_t * start = (uint32_t)directory;
  uint32_t * end = (uint32_t)copy;

  int i;
  for(i = 0; i < ENTRY_SIZE_DEC; i++)
    {
      if((unsigned int)start[i] & USED)
      {
        //TODO: add maping
        table_copy((start[i] & ~0xFFF), frame_alloc());
      }
    }

    return (void *)copy;
}

void * page_map(void * physaddr, void * virtualaddr, unsigned int flags)
{
  if (((unsigned int)virtualaddr & 0xFFF) || ((unsigned int)physaddr & 0xFFF))
  {
    return NULL;
  }

  uint32_t * pd = directory_get();
  uint32_t pdindex = DIR_GET((uint32_t)virtualaddr);

  if((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    uint32_t * page_table = table_create(3);
    table_map(pd, pdindex, page_table, 3);
  }

  uint32_t * pt = (uint32_t *)PHY_GET((uint32_t)pd[pdindex]);
  uint32_t ptindex = (uint32_t)TLB_GET((uint32_t)virtualaddr);

  /*if(((unsigned int)pt[ptindex]) & USED)
  {
    return NULL;
  }*/ //TODO Fix this.

  pt[ptindex] = (uint32_t)physaddr | (uint32_t)flags | USED;
  flush_tlb(virtualaddr);

  return (void *)virtualaddr;
}

int page_unmap(void * virtualaddr)
{
  if((unsigned int)virtualaddr & 0xFFF)
  {
    return 0;
  }

  uint32_t * pd = directory_get();
  uint32_t pdindex = DIR_GET((uint32_t)virtualaddr);

  if((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    return 1;
  }

  uint32_t * pt = (uint32_t *)PHY_GET((uint32_t)pd[pdindex]);
  uint32_t ptindex = (uint32_t)TLB_GET((uint32_t)virtualaddr);

  if((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    return 1;
  }

  pt[ptindex] &= 0x00000000;
  flush_tlb(virtualaddr);
  return 1;
}

void * page_physaddr(void * virtualaddr)
{
  if((unsigned int)virtualaddr & 0xFFF)
  {
    return NULL;
  }

  uint32_t * pd = directory_get();
  uint32_t pdindex = DIR_GET((uint32_t)virtualaddr);

  if((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    return NULL;
  }

  uint32_t * pt = (uint32_t *)PHY_GET((uint32_t)pd[pdindex]);
  uint32_t ptindex = (uint32_t)TLB_GET((uint32_t)virtualaddr);
  
  if((((unsigned int)pt[ptindex]) & USED) != USED)
  {
    return NULL;
  }
 
  return (void *)((pt[ptindex] & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}

void directory_load(void * addr)
{
  __asm__ __volatile__ ("mov %0, %%cr3":: "r"((uint32_t)addr));
}

void paging_enable(void)
{
  uint32_t cr0;
  __asm__ __volatile__ ("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000;
  __asm__ __volatile__ ("mov %0, %%cr0":: "r"(cr0));
}

uint32_t page_getsize(void)
{
  return PAGE_SIZE_HEX;
}

/////////////////////////// Heap lib stuff ///////////////////////

void * kmalloc(size_t size)
{
  if(paging_state)
  {
    return malloc(size);
  }

  return ponter_move_int(size, 0, 1);
}

void * krealloc(void * addr, size_t size)
{
  if(paging_state)
  {
    return realloc(addr, size);
  }

  return ponter_move_int(size, 0, 0);
}

void * kcalloc(size_t sizeold, size_t sizenew)
{
  if(paging_state)
  {
    return calloc(sizeold, sizenew);
  }

  return ponter_move_int(sizenew, 0, 1);
}

void kfree(void * addr)
{
  if(paging_state)
    free(addr);
}

int liballoc_lock()
{
  heap_lock();
}

int liballoc_unlock(void)
{
  heap_unlock();
}

void* liballoc_alloc(int n)
{
  return heap_alloc(((uint32_t)n) * page_getsize(), kernel_heap);
}

int liballoc_free(void* addr, int n)
{
  if(heap_free(addr, ((uint32_t)n) * page_getsize(), kernel_heap))
  {
    return 0;
  }

  return 1;
}