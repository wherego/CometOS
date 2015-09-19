#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>
#include <kernel/vga.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include <kernel/portio.h>

#define INDEX_GET(i) (i / 0x1000)
#define ADDR_GET(i) (i * 0x1000)

heap_t * heap_create(void * start, void * end, int rw, int usermode)
{
	heap_t * heap = (heap_t *)ponter_move_int(sizeof(heap_t *), 0, 0);

	heap->start = (uintptr_t)start;
	heap->start &= 0xFFFFF000;

	heap->end = (uintptr_t)end;
	heap->end &= 0xFFFFF000;
	if(heap->end != (uint32_t)end)
    	heap->end += page_getsize();

	heap->rw = rw ? 1:0;
	heap->user = usermode ? 1:0;
	heap->bitmap = bitmap_create((start - end) / 4);

	return heap;
}

int heap_delete(heap_t * heap)
{
	return 0;
}

void * heap_alloc(size_t size, heap_t * heap)
{
	if(heap == NULL)
		return NULL;

	if((unsigned int)size & 0xFFF)
	{
		printf("Not a page size\n");
		return NULL;
	}

	uint32_t number = (uint32_t)size;
	uint32_t index = bitmap_entrie_find(INDEX_GET(number), 0, heap->bitmap);
	uintptr_t addr = (uint32_t)heap->start - ADDR_GET(index);

	if(addr <= 0 || addr == NULL || addr > heap->start)
	{
		printf("-- NO FREE MEMORY --\n");
		return NULL;
	}

	bitmap_map(index, index + INDEX_GET(size), 1, heap->bitmap);

	uint32_t i = addr;
	while(i < addr + number)
	{
		volatile void * page = page_map(frame_alloc(), i, 3);
		if(page == NULL)
		{
			i -= page_getsize();
			while(i > addr)
			{
				void * phyaddr = page_physaddr(i);
				frame_free(phyaddr);
				page_unmap(i);
				i -= page_getsize();
			}

			return NULL;
		}

	    i += page_getsize();
	}

	return addr;
}

int heap_free(void * addr, size_t size, heap_t * heap)
{
	if(heap == 0)
		return 0;

	if((unsigned int)size & 0xFFF)
	{
		printf("Not a page size\n");
		return 0;
	}

	uint32_t number = (uint32_t)size;
	uint32_t index = INDEX_GET((heap->start - (uint32_t)addr));
	bitmap_map(index, index + INDEX_GET(size), 0, heap->bitmap);

	uint32_t i = (uint32_t)addr;
	while(i < addr + number)
	{
		frame_free(page_physaddr(i));
		page_unmap(i);
		i -= page_getsize();
	}

	return 1;
}

int heap_lock(void)
{
	cli();
	return 1;
}

int heap_unlock(void)
{
	sti();
	return 1;
}