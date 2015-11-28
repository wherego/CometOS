#ifndef _COMETOS_ARCH_I386_ARRAY_H
#define _COMETOS_ARCH_I386_ARRAY_H

#include <stdbool.h>

typedef struct
{
	uintptr_t *entries;
	uint32_t size;
}__attribute__((packed)) bitmap_t;

typedef struct array_node
{
	struct array_node * next;
	struct array_node * prev;
	void * value;
}__attribute__((packed)) array_node_t;

typedef struct
{
	array_node_t *start;
	array_node_t *end;
	size_t size;
}__attribute__((packed)) array_t;

bitmap_t * bitmap_create(uint32_t size);
int bitmap_delete(bitmap_t * bitmap);
void bitmap_map(uint32_t start, uint32_t end, int value, bitmap_t * bitmap);
uint32_t bitmap_entrie_set(uint32_t index, int value, bitmap_t * bitmap);
uint32_t bitmap_entrie_find(uint32_t size, int value, bitmap_t * bitmap);
int bitmap_entrie_check(uint32_t index, bitmap_t * bitmap);

array_t * array_create(void);
void array_delete(array_t * array);
void array_clear(array_t * array);

array_node_t * array_node_find(void * value, array_t * array);
void array_node_delete(array_node_t * node, array_t * array);
void array_node_insert(array_node_t * node, array_t * array);
array_node_t * array_pop(array_t * array);

#endif