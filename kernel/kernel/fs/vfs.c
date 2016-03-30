#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/fs/vfs.h>

#if defined(__i386__)
#include "../arch/i386/mem/paging.h"
#endif

uint32_t vfs_read(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	if(node->read != 0)
		return node->read(node, offset, size, buffer);
	else
		return 0;
}

uint32_t vfs_write(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	if(node->write != 0)
		return node->write(node, offset, size, buffer);
	else
		return 0;
}

void vfs_open(file_t *node, uint8_t read, uint8_t write)
{
	if(node->open != 0)
		return node->open(node);
}

void vfs_close(file_t *node)
{
	if(node->close != 0)
		return node->close(node);
}

struct dir_entry * vfs_readdir(file_t *node, uint32_t index)
{
	if((node->flag & 0x7) == FS_DIRECTORY && node->readdir != 0)
		return node->readdir(node, index);
	else
		return 0;
}

file_t * vfs_finddir(file_t *node, char *name)
{
	if((node->flag & 0x7) == FS_DIRECTORY && node->finddir != 0)
		return node->finddir(node, name);
	else
		return 0;
}