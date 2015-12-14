#include <stdint.h>
#include <string.h>

#include <kernel/array.h>

#if defined(__i386__)
#include "../arch/i386/mem/paging.h"
#endif

//TODO: Make array and bitmap fucntions use a malloc fuction.

bitmap_t * bitmap_create(uint32_t size)
{
	bitmap_t * bitmap = (bitmap_t *)kmalloc(sizeof(bitmap_t));

	bitmap->size = size;
	bitmap->entries = kmalloc(bitmap->size / 0x20);

	bitmap_map(0, bitmap->size, 0, bitmap);

	return bitmap;
}

uint32_t bitmap_entrie_set(uint32_t index, int value, bitmap_t * bitmap)
{
	return bitmap->entries[index] = value; 
}

int bitmap_entrie_check(uint32_t index, bitmap_t * bitmap)
{
	return (int)bitmap->entries[index];
}

uint32_t bitmap_entrie_find(uint32_t size, int value, bitmap_t * bitmap)
{
	uint32_t i = 0;

	while(i <= bitmap->size)
	{
		if(bitmap->entries[i] == value)
		{
			uint32_t x = 0;
			while(bitmap->entries[i + x] == value)
			{
				x++;
				if(x >= size)
				{
					return i;
				}
			}
		}

		i++;
	}

	return -1;
}

void bitmap_map(uint32_t start, uint32_t end, int value, bitmap_t * bitmap)
{
	uint32_t i = start;

	if(start < 0 || end > bitmap->size)
	{
		return;
	}

	while(i < end)
	{
		bitmap_entrie_set(i, value, bitmap);
		i++;
	}
}

int bitmap_delete(bitmap_t * bitmap)
{
	kfree(bitmap);
	return 1;
}

//==============================| Array |==============================//

array_t * array_create(void)
{
	array_t * array = kmalloc(sizeof(array_t));
	array->size = 0;
	array->start = NULL;
	array->end = NULL;

	return array;
}

void array_clear(array_t * array)
{
	array_node_t * node = array->start;

	while(node)
	{
		array_node_t * node_next = node->next;
		array_node_delete(node, array);
		node = node_next;
	}
}

void array_delete(array_t * array)
{
	array_clear(array);
	kfree(array);
}

array_node_t * array_node_find(void * value, array_t * array)
{
	if (array->start == NULL)
		return NULL;

	array_node_t * node = array->start;
	for (int i = 0; i <= array->size; ++i)
	{
		if(node->value == value)
		{
			return node;
		}

		node = node->next;
	}

	return NULL;
}

void array_node_delete(array_node_t * node, array_t * array)
{
	//if end node
	if(node == array->end)
	{
		if(array->start == node)
			array->end = NULL;
		else
		{
			node->prev->next = NULL;
			array->end = node->prev;
		}
	}

	//if start node
	if(node == array->start)
	{
		if(node->next)
		{
			array->start = node->next;
		}
		else
			array->start = NULL;
	}

	//if not at start or end
	if(node != array->end && node != array->start)
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	kfree(node);
	array->size--;
}

void array_node_insert(array_node_t * node, array_t * array)
{
	if (!array->end)
	{
		array->start = node;
	}
	else
	{
		array->end->next = node;
		node->prev = array->end;
	}

	array->end = node;
	array->size++;
}

array_node_t * array_pop(array_t * array)
{
	if(!array->end)
		return NULL;

	array_node_t * node = array->end;
	array_node_delete(array->end, array);
	return node;
}