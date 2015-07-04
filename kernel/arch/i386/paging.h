#ifndef _COMETOS_ARCH_I386_PAGING_H
#define _COMETOS_ARCH_I386_PAGING_H

enum page_flags
{
	PAGE_PRESENT = 1,
	PAGE_WRITABLE= 2,
	PAGE_USER = 4,
	PAGE_WRITETHOUGH = 8,
	PAGE_NOT_CACHEABLE = 0x10,
	PAGE_ACCESSED =	0x20,
	PAGE_DIRTY = 0x40,
	PAGE_PAT = 0x80,
	PAGE_CPU_GLOBAL = 0x100,
	PAGE_LV4_GLOBAL = 0x200,
   	PAGE_FRAME = 0x7FFFF000
};

enum table_flags
{
	TABLE_PRESENT =	1,
	TABLE_WRITABLE = 2,
	TABLE_USER = 4,
	TABLE_PWT = 8,
	TABLE_PCD = 0x10,
	TABLE_ACCESSED = 0x20,
	TABLE_DIRTY = 0x40,
	TABLE_4MB = 0x80,
	TABLE_CPU_GLOBAL = 0x100,
	TABLE_LV4_GLOBAL = 0x200,
   	TABLE_FRAME = 0x7FFFF000
};

__attribute__((unused))
static inline void flag_add (uint32_t * addr, uint32_t flag)
{
	*addr |= flag;
}

__attribute__((unused))
static inline void flag_remove (uint32_t * addr, uint32_t flag)
{
	*addr &= ~flag;
}

__attribute__((unused))
static inline void flush_tlb(unsigned long addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void paging_initialize(uint32_t mem_lower, uint32_t mem_upper);
//void page_fault(struct interrupt_context* int_ctx);

//page
uint32_t page_map(void * physaddr, void * virtualaddr, unsigned int flags);
uint32_t page_unmap(void * physaddr, void * virtualaddr);
void * page_physaddr(void * virtualaddr);

//frame
uint32_t frame_test(void * addr);
void frame_set(void * addr);
void frame_free(void * addr);
void * frame_find();
void * frame_alloc();

void * ponter_move_int(uint32_t size, uint32_t * physaddr);

//directory
void * directory_get(void);
void * directory_create(unsigned int flags);
void directory_delete(void * directory_addr);

//table
void * table_create(unsigned int flags);
void * table_delete(void * table_addr);
void * table_map(void * directory, unsigned long pdindex, void * table_addr, unsigned int flags);
void table_unmap(void * directory, unsigned long pdindex);

#endif