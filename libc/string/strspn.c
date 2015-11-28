#include <string.h>

size_t strspn(const char * str1, const char * str2)
{
	const char * ptr;
	const char * temp;
	size_t size = 0;
	for (ptr = str1; *ptr != '\0'; ++ptr)
	{
		for (temp = str2; *temp != '\0'; ++temp)
		{
			if (*ptr == *temp)
			{
				break;
			}
		}

		if (*temp == '\0')
		{
			return size;
		}
		else
		{
			++size;
		}
	}

	return size;
}