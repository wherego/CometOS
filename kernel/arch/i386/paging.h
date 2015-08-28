#ifndef _COMETOS_ARCH_I386_PAGING_H
#define _COMETOS_ARCH_I386_PAGING_H

__attribute__((unused))
static inline void flush_tlb(unsigned long addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void * frame_alloc();
int frame_test(void * addr);
void frame_free();

void * table_create(unsigned int flags);
void table_unmap(void * directory, unsigned long pdindex);
void table_remove(void * table_addr);
void * table_copy(void * table, void * copy);

void * directory_create(uint32_t flags);
void directory_remove(void * directory_addr);
void directory_load(void * addr);
void * directory_copy(void * directory, void * copy);

void paging_initialize(uint32_t mem_lower, uint32_t mem_upper);
void * page_map(void * physaddr, void * virtualaddr, unsigned int flags);
int page_unmap(void * virtualaddr);
void * page_physaddr(void * virtualaddr);
uint32_t page_getsize(void);

int liballoc_lock();
int liballoc_unlock();
void * liballoc_alloc(int n);
int liballoc_free(void * addr, int n);

#endif