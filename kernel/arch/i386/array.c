#include <stdint.h>
#include <string.h>

#include "../arch/i386/array.h"

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