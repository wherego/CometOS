#include <stdint.h>
#include <string.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/ext2.h>
#include <kernel/fs/initrd.h>
#include <kernel/ui/log.h>
#include <kernel/array.h>

#if defined(__i386__)
#include "../arch/i386/mem/paging.h"
#endif

ext2_superblock_t * initrd_superblock;
file_t * initrd_root;

void initrd_initialize(void * addr)
{
	//Setup root dir node
	initrd_root = (file_t *)kmalloc(sizeof(file_t));
	initrd_root->name[0] = '/';
	initrd_root->name[1] = '\0';
	initrd_root->mask = 0;
	initrd_root->uid = 0;
	initrd_root->gid = 0;
	initrd_root->inode = 0;
	initrd_root->flag = FS_DIRECTORY;
	initrd_root->read = 0;
	initrd_root->write = 0;
	initrd_root->open = 0;
	initrd_root->close = 0;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
	initrd_root->ptr = 0;
	initrd_root->impl = 0;

	log_print(NOTICE, "Initrd");
	initrd_superblock = (ext2_superblock_t *)((uintptr_t)addr + EXT2_BLOCK_SIZE_MIN );
}

uint32_t initrd_read(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{

}

struct dir_entry * initrd_readdir(file_t *node, uint32_t index)
{
	
}

file_t *initrd_finddir(file_t *node, char *name)
{
}