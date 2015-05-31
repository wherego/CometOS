#include <stdint.h>
#include <string.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include <kernel/portio.h>

#define PAGESIZE_DEC 4096
#define PAGESIZE_HEX 0x1000
#define TABESIZE_DEC 1024
#define TABLESIZE_HEX 0x400
#define ADDRSIZE_HEX 0x20
#define ADDRSIZE_DEC 32
#define FREE 0
#define USED 0x1

extern void *end_kernel;
uintptr_t kernel_pointer = (uintptr_t)&end_kernel;

uint32_t frame_number;
uint32_t *frame_bitmap;

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

uint32_t page_directory[TABESIZE_DEC] __attribute__((aligned(PAGESIZE_DEC)));
uint32_t first_page_table[TABESIZE_DEC] __attribute__((aligned(PAGESIZE_DEC)));

void page_fault(struct interrupt_context* int_ctx)
{
  uint32_t addr;
  asm volatile("mov %%cr2, %0" : "=r" (addr));

  /*int present = !(int_ctx->err_code & 0x1); // Page not present
  int rw = int_ctx.err_code & 0x2; // Write operation?
  int us = int_ctx.err_code & 0x4; // Processor was in user-mode?
  int reserved = int_ctx.err_code & 0x8; // Overwritten CPU-reserved bits of page entry?
  int id = int_ctx.err_code & 0x10; // Caused by an instruction fetch?*/

  int present = ((int)int_ctx & 0x1);
  int rw = ((int)int_ctx & 0x2);
  int us = ((int)int_ctx & 0x4);
  int reserved = ((int)int_ctx & 0x8);
  int id = ((int)int_ctx & 0x10);

  printf("\n-- Pagefult --\n\n");
  printf("Addr: 0x%x\n", addr);
  printf("Present: %i\n", present);
  printf("rw: %i\n", rw);
  printf("us: %i\n", us);
  printf("reserved: %i\n", reserved);
  printf("id: %i\n\n", id);

  log_print(CRIT, "Kernel hlt");
  asm("hlt");
}

void paging_initialize(uint32_t mem_lower, uint32_t mem_upper)
{
  frame_number = mem_upper / 4;
  frame_bitmap = ponter_move_int((frame_number / ADDRSIZE_HEX), 0);

  int i;
  for(i = 0; i < TABESIZE_DEC; i++)
  {
    page_directory[i] = 0x00000002;
    first_page_table[i] = (i * PAGESIZE_HEX) | 3; 
  }

  page_directory[0] = ((unsigned int)first_page_table) | 3;

  loadPageDirectory(page_directory);
  enablePaging();

  isr_install_handler(14, page_fault);

  while(i < kernel_pointer)
  {
    page_map(frame_alloc(), i*PAGESIZE_HEX, 0);
    i += PAGESIZE_HEX;
  }
}

void frame_set(void * addr)
{
   uint32_t index = (uint32_t)addr / PAGESIZE_HEX;
   frame_bitmap[index / ADDRSIZE_HEX] |= (USED << (index % ADDRSIZE_HEX));
}

void frame_free(void * addr)
{
   uint32_t index = (uint32_t)addr / PAGESIZE_HEX;
   frame_bitmap[index / ADDRSIZE_HEX] &= ~(USED << (index % ADDRSIZE_HEX));
}

uint32_t frame_test(void * addr)
{
   uint32_t index = (uint32_t)addr / PAGESIZE_HEX;
   return frame_bitmap[index] & (USED << (index % ADDRSIZE_HEX));
}

void * frame_find()
{
  int i, x;
  for (i = 0; i < (frame_number / ADDRSIZE_DEC); i++) 
  {
    if (frame_bitmap[i] != 0xFFFFFFFF)
    {
      for (x = 0; x < ADDRSIZE_DEC; x++)
      {
        if (frame_bitmap[i] & (USED << x))
        {
          return i * ADDRSIZE_DEC + x;
        }
      }
    }
  }

    return -1;
}

void * ponter_move_int(uint32_t size, uint32_t * physaddr)
{
  if(physaddr)
  {
    *physaddr = kernel_pointer;
  }

  uint32_t i = kernel_pointer;
  kernel_pointer += size;

  return (void *)i;
}

void * frame_alloc()
{
  uint32_t index = frame_find();
  if(index == -1)
  {
    log_print(ERROR, "Paging: No free frames.");
    return NULL;
  }

  frame_set(index * PAGESIZE_HEX);
  return ((void *)(index * PAGESIZE_HEX));
}

void * directory_get(void)
{
  uint32_t cr3;
  asm volatile("mov %%cr3, %0": "=r"(cr3));

  return (void *)cr3;
}

uint32_t page_map(void * physaddr, void * virtualaddr, unsigned int flags)
{
    // Check if not aligned
  if (((unsigned int)virtualaddr & 0xFFF) || ((unsigned int)physaddr & 0xFFF))
  {
    printf("not aligned");
    return NULL;
  }

  unsigned long page_index = (unsigned long)virtualaddr / PAGESIZE_HEX;
  unsigned long pdindex = (unsigned long)page_index / TABLESIZE_HEX;
  unsigned long ptindex = (unsigned long)page_index % TABLESIZE_HEX;

  unsigned long * pd = (unsigned long *)directory_get(); 
  // Seek out or create Pagetable
  if ((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    //create table
    uint32_t * page_table = (uint32_t)frame_alloc();

    int i;
    for(i = 0; i < TABESIZE_DEC; i++)
    {
     page_table[i] = (i * PAGESIZE_HEX) | 3;
    }

    pd[pdindex] = ((unsigned long)page_table) | 3 ;
  }

  unsigned long * pt = ((unsigned long *)pd) + (TABLESIZE_HEX * pdindex);
  if ((unsigned int)pt[ptindex] & USED)
  {
    return NULL;
  }

  pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | USED;
  flush_tlb(virtualaddr);

  return (void *)pt[ptindex];
}

void * page_physaddr(void * virtualaddr)
{
  unsigned long page_index = (unsigned long)virtualaddr / PAGESIZE_HEX;
  unsigned long pdindex = (unsigned long)page_index / TABLESIZE_HEX;
  unsigned long ptindex = (unsigned long)page_index % TABLESIZE_HEX;
 
  unsigned long * pd = (unsigned long *)directory_get();

  //Check whether the PD entry is present.
  if((((unsigned int)pd[pdindex]) & USED) != USED)
  {
    return NULL;
  }
 
  unsigned long * pt = ((unsigned long *)0xFFC00000) + (TABLESIZE_HEX * pdindex);

  //Check whether the PT entry is present.
  if((((unsigned int)pt[ptindex]) & USED) != USED)
  {
    return NULL;
  }
 
  return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}