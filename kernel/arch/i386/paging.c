#include <stdint.h>
#include <string.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include <kernel/portio.h>

#define INDEX_FROM_BIT(b) (b / 0x20)
#define OFFSET_FROM_BIT(b) (b % 0x20)

extern uint32_t placement_address;
extern heap_t *kheap;

uint32_t *frames;
uint32_t nframes;

page_directory_t *kernel_directory=0;
page_directory_t *current_directory=0;

static void set_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

static uint32_t test_frame(uint32_t frame_addr)
{
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   return (frames[idx] & (0x1 << off));
}

void dma_frame(page_t *page, int is_kernel, int is_writeable, uintptr_t address)
{
  /* Page this address directly */
  page->present = 1;
  page->rw = (is_writeable) ? 1 : 0;
  page->user = (is_kernel) ? 0 : 1;
  page->frame = address / 0x1000;
}

static uint32_t first_frame()
{
   uint32_t i, j;
   for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
   {
       if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
       {
           // at least one bit is free here.
           for (j = 0; j < 32; j++)
           {
               uint32_t toTest = 0x1 << j;
               if ( !(frames[i]&toTest) )
               {
                   return i*4*8+j;
               }
           }
       }
   }
}

uintptr_t memory_use()
{
  uintptr_t ret = 0;
  uint32_t i, j;

  for (i = 0; i < INDEX_FROM_BIT(nframes); ++i)
  {
    for (j = 0; j < 32; ++j)
    {
      uint32_t testFrame = 0x1 << j;

      if (frames[i] & testFrame)
      {
        ret++;
      }
    }
  }

  return ret * 4;
}

uintptr_t memory_total()
{
  return nframes * 4;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
   if (page->frame != 0)
   {
       return;
   }
   else
   {
       uint32_t idx = first_frame();
       if (idx == (uint32_t)-1)
       {
           log_print(ERROR, "No free frames!");
       }
       set_frame(idx*0x1000);
       page->present = 1;
       page->rw = (is_writeable)?1:0;
       page->user = (is_kernel)?0:1;
       page->frame = idx;
   }
}

void free_frame(page_t *page)
{
   uint32_t frame;
   if (!(frame=page->frame))
   {
       return;
   }
   else
   {
       clear_frame(frame);
       page->frame = 0x0;
   }
}

void paging_initialize(uint32_t mem_upper, uint32_t mem_lower)
{
   nframes = mem_upper / 4;
   frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
   memset(frames, 0, INDEX_FROM_BIT(nframes));

   kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
   memset(kernel_directory, 0, sizeof(page_directory_t));
   current_directory = kernel_directory;

   int i = 0;
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        get_page(i, 1, kernel_directory);

   i = 0;
   while (i < placement_address + 0x3000)
   {
       alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
       i += 0x1000;
   }

   for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);

   isr_install_handler(14, page_fault);
   switch_page_directory(kernel_directory);
   log_print(NOTICE, "Paging");

   kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
   log_print(NOTICE, "Heap");
}

void switch_page_directory(page_directory_t *dir)
{
   current_directory = dir;
   asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
   uint32_t cr0;
   asm volatile("mov %%cr0, %0": "=r"(cr0));
   cr0 |= 0x80000000; // Enable paging
   asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir)
{
   address /= 0x1000;
   uint32_t table_idx = address / 1024;
   if (dir->tables[table_idx])
   {
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else if(make)
   {
       uint32_t tmp;
       dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
       memset(dir->tables[table_idx], 0, 0x1000);
       dir->tablesPhysical[table_idx] = tmp | 0x7;
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else
   {
       return 0;
   }
} 

void page_fault(void)
{
  uint32_t faulting_address;
  asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

  /*int present = !(int_ctx->err_code & 0x1);
  int rw = int_ctx->err_code & 0x2;
  int user = int_ctx->err_code & 0x4;
  int reserved = int_ctx->err_code & 0x8;
  int id = int_ctx->err_code & 0x10;*/

  log_print(ERROR, "Page fault! at: 0x%x", faulting_address);
  /*printf("------------");
  printf("present:%i\n", present);
  printf("user:%i\n", user);
  printf("reserved:%i\n", reserved);
  printf("id:%i\n", id);
  printf("------------");*/
}