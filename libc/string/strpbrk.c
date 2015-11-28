#include <string.h>

char * strpbrk(const char * str1, const char * str2)
{
	while(*str1 != '\0')
	{
		const char *acc = str2;
		while(*acc != '\0')
		{
			if(*acc++ == *str1)
				return (char *)str1;
		}

		++str1;
	}

	return NULL;
}