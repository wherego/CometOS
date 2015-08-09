#ifndef _COMETOS_ARCH_I386_ARRAY_H
#define _COMETOS_ARCH_I386_ARRAY_H

typedef struct
{
	uintptr_t *entries;
	uint32_t size;
}__attribute__((packed)) bitmap_t;

typedef struct
{
	uintptr_t *entries;
	uint32_t size;
}__attribute__((packed)) array_t;

bitmap_t bitmap_create(uint32_t size);
int bitmap_delete(bitmap_t bitmap);
void bitmap_map(uint32_t start, uint32_t end, int value, bitmap_t bitmap);
uint32_t bitmap_entrie_set(uint32_t index, int value, bitmap_t bitmap);
uint32_t bitmap_entrie_find(uint32_t size, int value, bitmap_t bitmap);
int bitmap_entrie_check(uint32_t index, bitmap_t bitmap);

#endif