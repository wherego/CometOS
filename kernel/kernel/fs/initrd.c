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

file_t * initrd_root;
ext2_t * ext2_initrd;

void initrd_initialize(void * addr)
{
	ext2_initrd->superblock = ext2_superblock_get(addr);
	struct ext2_inode * inode = ext2_inode_get(EXT2_INODE_ROOT, ext2_initrd->superblock);

	#ifdef DEBUG
		printf("Mounting File System\n");
	#endif

	//Setup root dir node
	initrd_root = (file_t *)kmalloc(sizeof(file_t));
	initrd_root->name[0] = '/';
	initrd_root->name[1] = '\0';
	initrd_root->mask = inode->type & 0xFFF;
	initrd_root->uid = inode->uid;
	initrd_root->gid = inode->gid;
	initrd_root->inode = EXT2_INODE_ROOT;
	initrd_root->flag |= ext2_inode_typetofile(inode);
	initrd_root->size = inode->size;
	initrd_root->read = NULL;
	initrd_root->write = NULL;
	initrd_root->open = NULL;
	initrd_root->close = NULL;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
	initrd_root->ptr = NULL;
	initrd_root->impl = NULL;

	#ifdef DEBUG
		printf("####################| EXT2 Debug | ####################\n");
		printf("SuperBlock: 0x%x\n", ext2_initrd->superblock);
		printf("superblock Validate: %s\n", ext2_superblock_validate(ext2_initrd->superblock)?"Yes":"No");
		printf("#######################################################\n");
	#endif

	log_print(NOTICE, "Initrd");
}

uint32_t initrd_read(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{

}

struct dir_entry * initrd_readdir(file_t *node, uint32_t index)
{
	struct ext2_inode * inode = ext2_inode_get(node->inode, ext2_initrd->superblock);
	struct ext2_dir_entry * ent = ext2_direntry_get(index, inode, ext2_initrd->superblock);

	if(!inode || !ent)
		return NULL;

	struct dir_entry * dir_entry = kmalloc(sizeof(struct dir_entry));

	//Copy Name
	memcpy(&dir_entry->name, &ent->name, ent->name_len);
	dir_entry->name[ent->name_len] = '\0';

	//Copy Inode
	dir_entry->inode = ent->inode;

	return dir_entry;
}

file_t *initrd_finddir(file_t *node, char *name)
{
}