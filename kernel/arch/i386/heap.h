#ifndef _COMETOS_ARCH_I386_HEAP_H
#define _COMETOS_ARCH_I386_HEAP_H

typedef struct node
{
	uint32_t magic : 8;
	uintptr_t next : 32;
	uintptr_t prev : 32;
	uint32_t size : 32;

}__attribute__((packed)) node_t;

typedef struct heap
{
	uintptr_t start;
	uint32_t size;
	uint32_t max;
	uint32_t rw : 1;
	uint32_t user : 1;
	uintptr_t * list;
}__attribute__((packed)) heap_t;

heap_t * heap_create(void * start, uint32_t maxsize, int rw, int usermode, void * list, void * addr);
int heap_delete(heap_t * heap);
int heap_addsize(uint32_t size, heap_t * heap);
int heap_subsize(uint32_t size, heap_t * heap);
void * heap_alloc(int number, heap_t * heap);
int heap_free(void * addr, int number);

void * page_find(int number, heap_t * heap);
void page_set(void * addr, heap_t * heap);

#endif