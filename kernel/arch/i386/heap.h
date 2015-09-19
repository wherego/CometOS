#ifndef _COMETOS_ARCH_I386_HEAP_H
#define _COMETOS_ARCH_I386_HEAP_H

#include "../arch/i386/array.h"

typedef struct heap
{
	uintptr_t start; //start addr of heap
	uint32_t end; //end addr of heap
	uint32_t rw : 1;
	uint32_t user : 1;
	bitmap_t * bitmap; //array of frames
}__attribute__((packed)) heap_t;

heap_t * heap_create(void * start, void * end, int rw, int usermode);
int heap_delete(heap_t * heap);
void * heap_alloc(size_t size, heap_t * heap);
int heap_free(void * addr, size_t size, heap_t * heap);
int heap_lock(void);
int heap_unlock(void);

#endif