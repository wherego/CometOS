#include <stddef.h>
#include <stdlib.h>

size_t lfind(const char * str1, const char str2)
{
	size_t i = 0;
	while ( str1[i] != str2)
	{
		i++;
	}
	
	return (size_t)(str1) + i;
}