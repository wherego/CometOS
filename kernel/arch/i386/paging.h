#ifndef _COMETOS_ARCH_I386_PAGING_H
#define _COMETOS_ARCH_I386_PAGING_H

typedef struct page
{
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
   page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
   page_table_t *tables[1024];

   uint32_t tablesPhysical[1024];
   uint32_t physicalAddr;
} page_directory_t;

void paging_initialize(uint32_t mem_upper, uint32_t mem_lower);
void switch_page_directory(page_directory_t *new);
page_t *get_page(uint32_t address, int make, page_directory_t *dir);

uint32_t kmalloc_int(uint32_t sz, int align, uint32_t *phys);
uint32_t kmalloc_a(uint32_t sz);
uint32_t kmalloc_p(uint32_t sz, uint32_t *phys);
uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys);
uint32_t kmalloc(uint32_t sz);

void dma_frame(page_t * page, int, int, uintptr_t);

uintptr_t memory_use();
uintptr_t memory_total();

void page_fault(void);

#endif