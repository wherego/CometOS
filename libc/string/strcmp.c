#include <string.h>
	
int strcmp(char *a, char *b)
{
	int i = 0;
	while(a[i] != '\0' && b[i] != '\0')
	{
		if(a[i] != b[i])
		{
			if((a[i] == '\0' && b[i] != '\0') || (a[i] != '\0' && b[i] == '\0'))
				return 1;
			else
				break;
		}

		i++;
	}
}