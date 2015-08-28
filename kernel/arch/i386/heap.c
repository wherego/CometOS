#include <stdint.h>
#include <string.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include <kernel/portio.h>

#define INDEX_GET(i) (i / 0x1000)
#define ADDR_GET(i) (i * 0x1000)

heap_t * heap_create(void * start, void * end, int rw, int usermode)
{
	heap_t * heap = (heap_t *)ponter_move_int(sizeof(heap_t *), 0, 1);

	heap->start = (uintptr_t)start;
	heap->end = (uintptr_t)end;
	heap->rw = rw ? 1:0;
	heap->user = usermode ? 1:0;
	heap->bitmap = bitmap_create((start - end) / 4);

	return heap;
}

int heap_delete(heap_t * heap)
{
	return 0;
}

void * heap_alloc(int number, heap_t * heap)
{
	if(heap == NULL)
	{
		return NULL;
	}

	uint32_t index = bitmap_entrie_find((uint32_t)number, 0, heap->bitmap);
	uint32_t addr = (uint32_t)heap->start - ADDR_GET(index);

	if(index != -1)
	{
		bitmap_map(index, index + number, 0x1, heap->bitmap);

		uint32_t i = addr;
  		while(i < addr + ADDR_GET(number))
  		{
    		page_map(frame_alloc(), addr + i, 3);

    		i += page_getsize();
  		}

  		printf("addr:%x\n", addr);
		return (void *)addr;
	}

	return NULL;
}

int heap_free(void * addr, int number, heap_t * heap)
{
	if(heap == NULL || addr < 0)
	{
		return NULL;
	}

	uint32_t index = ((uint32_t)heap->start - (uint32_t)addr) - (uint32_t)heap->end;

	bitmap_map(INDEX_GET(index), INDEX_GET(index) + number, 0x0, heap->bitmap);

	uint32_t i = addr;
  	while(i < addr + ADDR_GET(number))
  	{
  		frame_free(page_physaddr(addr + i));
		page_unmap(addr + i);

    	i += page_getsize();
  	}

	return 1;	
}

int heap_lock(void)
{
	cli();
	return 0;
}

int heap_unlock(void)
{
	sti();
	return 0;
}