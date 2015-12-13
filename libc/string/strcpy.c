#include <string.h>

char* strcpy(char* restrict dstptr, const char* restrict srcptr)
{
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	int size = strlen(srcptr);
	for(size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}