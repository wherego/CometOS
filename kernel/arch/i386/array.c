#include <stdint.h>
#include <string.h>

#include "../arch/i386/array.h"
#include "../arch/i386/paging.h"

bitmap_t bitmap_create(uint32_t size)
{
	bitmap_t bitmap;
	//bitmap = (bitmap_t)ponter_move_int(sizeof(bitmap_t), 0, 1);

	bitmap.size = size;
	bitmap.entries = ponter_move_int((bitmap.size / 0x20), 0, 1);

	//memset(bitmap.entries, 0, bitmap.size / 0x20);
	bitmap_map(0, bitmap.size, 0, bitmap);

	return bitmap;
}

uint32_t bitmap_entrie_set(uint32_t index, int value, bitmap_t bitmap)
{
	return bitmap.entries[index] = value; 
}

int bitmap_entrie_check(uint32_t index, bitmap_t bitmap)
{
	return (int)bitmap.entries[index];
}

uint32_t bitmap_entrie_find(uint32_t size, int value, bitmap_t bitmap)
{
	uint32_t i = 0;

	while(i <= bitmap.size)
	{
		if(bitmap.entries[i] == value)
		{
			uint32_t x = 0;
			while(bitmap.entries[i + x] == value)
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

void bitmap_map(uint32_t start, uint32_t end, int value, bitmap_t bitmap)
{
	uint32_t i = start;

	if(start < 0 || end > bitmap.size)
	{
		return;
	}

	while(i <= end)
	{
		bitmap_entrie_set(i, value, bitmap);
		i++;
	}
}

array_t * array_create(uint32_t order)
{
	//array_t * array = ponter_move_int(sizeof(array_t), 0, 1);
	array_t * array = frame_alloc();
	page_map(array, array, 3);

	array->start = NULL;
	array->end = NULL;
	array->size = 0;
	array->order = order ? 1:0;

	return array;
}

array_node_t * array_node_create(uint32_t index, void * value)
{
	//array_node_t * node = ponter_move_int(sizeof(array_node_t), 0, 1);
	array_node_t * node = frame_alloc();
	page_map(node, node, 3);

	node->next = NULL;
	node->prev = NULL;
	node->index = index;
	node->value = value;

	return node;
}

array_node_t * array_node_find(uint32_t index, array_t * array)
{
	array_node_t * node = array->start;
	int end = 0;

	if(array->order)
	{
		while(end == 0)
		{
			if(node->index == index)
			{
				return node;
			}
			else if(node->index < index)
			{
				if(node->prev == NULL)
				{
					return NULL;
				}

				return node->prev;
			}
			
			if (node->next != NULL)
				node = node->next;
			else
				end = 1;
		}
	}
	else
	{
		while(end == 0)
		{
			if(node->index >= index)
			{
				return node;
			}
			
			if (node->next != NULL)
				node = node->next;
			else
				end = 1;
		}
	}

	return NULL;
}

array_node_t * array_node_sort(array_node_t * node, array_t * array)
{
	if (node == NULL || array == NULL)
	{
		return NULL;
	}

	array_node_t * index = array->start;
	int end = 0;

	if(array->order)
	{
		if(array->start == NULL)
		{
			array->start = node;
			array->end = node;

			return node;
		}

		while(end == 0)
		{
			if(node->index > index->index)
			{
				//found the spot
				node->next = index;
				node->prev = index->prev;

				index->prev->next = node;
				index->prev = node;

				array->size ++;

				printf("test");

				return node;
			}

			if(index->next != NULL)
				index = index->next;
			else
				end = 1;
		}
		//add to end
		node->next = NULL;
		node->prev = index;
		index->next = node;

		array->end = node;
		array->size ++;

		return node;
	}
	else
	{
		if(array->start == NULL)
		{
			array->start = node;
			array->end = node;

			return node;
		}

		while(end == 0)
		{
			if(node->index < index->index)
			{
				//found the spot
				node->next = index;
				node->prev = index->prev;

				index->prev->next = node;
				index->prev = node;

				array->size ++;

				printf("test");

				return node;
			}

			if(index->next != NULL)
				index = index->next;
			else
				end = 1;
		}
		//add to end
		node->next = NULL;
		node->prev = index;
		index->next = node;

		array->end = node;
		array->size ++;

		return node;
	}
}

int array_node_delete(array_node_t * node)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;

	void * addr = page_physaddr(node);
	page_unmap(node);
	frame_free(addr);

	return 1;
}

int array_delete(array_t * array)
{
	array_node_t * node = array->start->next;

	while(node->index != array->end->index)
	{
		if(!(array_node_delete(node->prev)))
		{
			return 0;
		}

		node = node->next;
	}

	if(array_node_delete(node))
	{
		void * addr = page_physaddr(array);
		page_unmap(array);
		frame_free(addr);

		return 1;
	}

	return 0;
}