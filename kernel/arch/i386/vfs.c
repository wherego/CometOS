#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vfs.h"
#include "../arch/i386/liballoc.h"
#include "../arch/i386/array.h"
#include <kernel/multiboot.h>

file_t *fs_root = 0;

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

char * vfs_path(char *cwd, char *input)
{
	array_t * array = array_create();

	if(strlen(input) && input[0] != '/')
	{
		char * path = kmalloc((strlen(cwd) + 1) * sizeof(char));
		memcpy(path, cwd, strlen(cwd) + 1);

		char * pch;
		char * save;
		pch = strtok_r(path,"/",&save);

		while (pch != NULL) {
			char * s = kmalloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			array_node_insert(array, s);
			pch = strtok_r(NULL,"/",&save);
		}
		kfree(path);
	}

	char * path = kmalloc((strlen(input) + 1) * sizeof(char));
	memcpy(path, input, strlen(input) + 1);
	char * pch;
	char * save;
	pch = strtok_r(path,"/",&save);
	while (pch != NULL) {
		if (!strcmp(pch,"..")) {
			array_node_t * n = array_pop(array);
			if (n) {
				kfree(n->value);
				kfree(n);
			}
		} else if (!strcmp(pch,".")) {
			/* pass */
		} else {
			char * s = kmalloc(sizeof(char) * (strlen(pch) + 1));
			memcpy(s, pch, strlen(pch) + 1);
			array_node_insert(s, array);
		}
		pch = strtok_r(NULL, "/", &save);
	}
	kfree(path);

	size_t size = 0;

	array_node_t * temp = array->start;
	for(int i = 0; i <= array->size; ++i)
	{
		size += strlen(temp->value) + 1;
	}

	char * arrayput = kmalloc(sizeof(char) * (size + 1));
	char * arrayput_offset = arrayput;
	if (size == 0) {
		arrayput = realloc(arrayput, sizeof(char) * 2);
		arrayput[0] = '/';
		arrayput[1] = '\0';
	}
	else
	{
		array_node_t * temp = array->start;
		for(int i = 0; i <= array->size; ++i)
		{
			arrayput_offset[0] = '/';
			arrayput_offset++;
			memcpy(arrayput_offset, temp->value, strlen(temp->value) + 1);
			arrayput_offset += strlen(temp->value);
			temp = temp->next;
		}
	}

	array_delete(array);
	kfree(array);

	return arrayput;
}