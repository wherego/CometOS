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
	if(ext2_initrd->superblock->state == EXT2_STATE_ERROR)
		ext2_error(ext2_initrd->superblock);

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
		
		file_t * file = initrd_finddir(initrd_root, "test.txt");
		if(file)
		{
			char * buffer = kmalloc(file->size);
			uint32_t read = initrd_read(file, 0, file->size, buffer);

			if(read)
			{
				printf("-- Finding test file - test.txt --\n");
				printf("file name: %s\n", file->name);
				printf("file uid: %i\n", file->uid);
				printf("file gid: %i\n", file->gid);
				printf("file flag: %i\n", file->flag);
				printf("file inode: %i\n", file->inode);
				printf("file size(bytes): %i\n", file->size);
				printf("file impl: %i\n", file->impl);

				printf("===== Contents =====\n");
				printf("%s\n", buffer);
				printf("====================\n");
				kfree(buffer);
			}
			else
				printf("Test file read failed testing\n");
		}
		else
			printf("Could not find test file\n");

		printf("#######################################################\n");
	#endif

	log_print(NOTICE, "Initrd");
}

uint32_t initrd_read(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	struct ext2_inode * inode = ext2_inode_get(node->inode, ext2_initrd->superblock);
	return ext2_read(offset, buffer, size, inode, ext2_initrd->superblock);
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

file_t * initrd_finddir(file_t *node, char *name)
{
	struct ext2_inode * inode = ext2_inode_get(node->inode, ext2_initrd->superblock);
	void * block = (void *)ext2_inodeblock_get(0, inode, ext2_initrd->superblock);
	uint32_t index = 0;

	if(!node || !(inode->type & EXT2_IFDIR) || !block)
		return NULL;

	while(index < inode->size)
	{
		struct ext2_dir_entry * ent = (struct ext2_dir_entry *)((uintptr_t)block + index);

		if(strlen(name) == ent->name_len && !strcmp(&ent->name, name))
		{
			inode = ext2_inode_get(ent->inode, ext2_initrd->superblock);

			//Setup file
			file_t * file = (file_t *)kmalloc(sizeof(file_t));

			if(strlen(ent->name) > 128) // check that name is not too long
				return NULL;

			strcpy(&file->name, &ent->name);
			file->mask = inode->type & 0xFFF;
			file->uid = inode->uid;
			file->gid = inode->gid;
			file->inode = ent->inode;
			file->flag |= ext2_inode_typetofile(inode);
			file->size = inode->size;
			file->read = &initrd_read;
			file->write = NULL;
			file->open = NULL;
			file->close = NULL;
			file->readdir = &initrd_readdir;
			file->finddir = &initrd_finddir;
			file->ptr = NULL;
			file->impl = NULL;

			return file;
		}

		index += ent->rec_len;
	}

	return NULL;
}