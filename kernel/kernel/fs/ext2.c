#include <stdint.h>
#include <string.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/ext2.h>
#include <kernel/ui/log.h>

struct ext2_superblock * ext2_superblock_get(void * addr)
{
	//SuperBlock
	struct ext2_superblock * superblock = addr + EXT2_SUPERBLOCK_OFFSET;
	if(!ext2_superblock_validate(superblock))
	{
		return NULL;
	}

	superblock->inode_size = 128;
	return superblock;
}

int ext2_superblock_validate(struct ext2_superblock * superblock)
{
	if(superblock->magic == EXT2_MAGIC)
		return 1;
	else
		return 0;
}

char ext2_osid_get(struct ext2_superblock * superblock)
{
	switch(superblock->osid)
	{
		case EXT2_OSID_LINUX:
			return "linux";
		break;

		case EXT2_OSID_GNUGURD:
			return "gnu-gurd";
		break;

		case EXT2_OSID_MASIX:
			return "masix";
		break;

		case EXT2_OSID_FREEBSD:
			return "free-bsd";
		break;

		default:
			return "unknown";
		break;
	}

	return 0;
}

void ext2_error(struct ext2_superblock * superblock)
{
	switch(superblock->error)
	{
		case EXT2_ERROR_IGNORE:
			//Ignore
		break;

		case EXT2_ERROR_REMOUNT:
			//Remount
		break;

		case EXT2_ERROR_PANIC:
			log_print(CRIT, "Ext2 superblock panic error");
			asm("hlt");
		break;
	}
}

uint32_t ext2_inode_typetofile(struct ext2_inode * inode)
{
	if((inode->type & EXT2_IFLNK) == EXT2_IFLNK)
		return FS_SYMLINK;

	if((inode->type & EXT2_IFREG) == EXT2_IFREG)
		return FS_FILE;

	if((inode->type & EXT2_IFBLK) == EXT2_IFBLK)
		return FS_BLOCKDEVICE;

	if((inode->type & EXT2_IFDIR) == EXT2_IFDIR)
		return FS_DIRECTORY;

	if((inode->type & EXT2_IFCHR) == EXT2_IFCHR)
		return FS_CHARDEVICE;

	if((inode->type & EXT2_IFIFO) == EXT2_IFIFO)
		return FS_PIPE;

	return 0;
}

void * ext2_blockgroup_get(uint32_t blockgroup, struct ext2_superblock * superblock)
{
	uint32_t offset = (EXT2_BLOCKSIZE_GET(superblock) * superblock->block_per_group) * blockgroup;
	return ((void *)superblock - EXT2_BLOCKSIZE_GET(superblock)) + offset;
}

struct ext2_blockgroup_descriptor * ext2_bg_des_get(uint32_t blockgroup, struct ext2_superblock * superblock)
{
	return ext2_blockgroup_get(blockgroup, superblock) + (EXT2_BLOCKSIZE_GET(superblock) * 2);
}

struct ext2_inode * ext2_inode_get(uint32_t inode, struct ext2_superblock * superblock)
{
	uint32_t block_group;
	uint32_t index;
	struct ext2_blockgroup_descriptor * bg_des;
	uint32_t inode_table_offset;
	void * inode_table;

	//Check that inode is not bigger then max
	if(inode > superblock->inode_total)
		return NULL;

	block_group = (inode - 1) / superblock->inode_per_group;

	//Check that block group is not bigger then max
	if(block_group > superblock->block_total / superblock->block_per_group)
		return NULL;

	bg_des = ext2_bg_des_get(block_group, superblock);
	index = (inode - 1) % superblock->inode_per_group;
	inode_table_offset = (EXT2_BLOCKSIZE_GET(superblock) * bg_des->inode_table_addr) - EXT2_SUPERBLOCK_OFFSET;
	inode_table = (void *)(uintptr_t)superblock + inode_table_offset;
	return (struct ext2_inode *)((uintptr_t)inode_table + superblock->inode_size * index);
}

void * ext2_inodeblock_get(uint32_t block, struct ext2_inode * inode, struct ext2_superblock * superblock)
{
	if(block < EXT2_INODE_BLOCK_TOTAL)
	{
		if(!inode->block[block])
			return NULL;

		return ext2_block_get(inode->block[block], superblock);
	}

	//Indirect
	if(block < EXT2_INODE_BLOCK_TOTAL + (EXT2_BLOCKSIZE_GET(superblock) / sizeof(uint32_t)))
	{
		uint32_t index = sizeof(uint32_t) * (block - EXT2_INODE_BLOCK_TOTAL);
		void * indirect_block = ext2_block_get(inode->block[12], superblock);

		if(!*(uint32_t*)((uintptr_t)indirect_block + index))
			return NULL;

		return ext2_block_get(*(uint32_t*)((uintptr_t)indirect_block + index), superblock);
	}

	//TODO: add 2 and 3 Indirect support

	return NULL;
}

void * ext2_block_get(uint32_t block, struct ext2_superblock * superblock)
{
	if(block <= 0)
		return (void *)superblock;
	else
		return (void *)((uintptr_t)superblock - EXT2_SUPERBLOCK_OFFSET) + (EXT2_BLOCKSIZE_GET(superblock) * block);
}

struct ext2_dir_entry * ext2_direntry_get(uint32_t index, struct ext2_inode * inode, struct ext2_superblock * superblock)
{
	void * block;
	uint32_t block_number = 0;
	uint32_t offset = 0;
	int i = 0;

	Find:
		block = ext2_inodeblock_get(block_number, inode, superblock);
		
		while(i < index)
		{
			if(offset > inode->size)
				return NULL;

			if(offset >= EXT2_BLOCKSIZE_GET(superblock))
			{
				block_number++;
				goto Find; //Next block
			}

			struct ext2_dir_entry * ent = (struct ext2_dir_entry *)((uintptr_t)block + offset);
			offset += ent->rec_len;
			i++;
		}

		if(i == index)
			return (struct ext2_dir_entry *)((uintptr_t)block + offset);

	return NULL;
}

uint32_t ext2_read(uint32_t offset, void * buffer, uint32_t size, struct ext2_inode * inode, struct ext2_superblock * superblock)
{
	if(!buffer || !inode || !superblock)
		return 0;

	uint32_t index = size;
	uint32_t block = 0;
	uint32_t block_offset = 0;

	if(offset)
	{
		block = offset / EXT2_BLOCKSIZE_GET(superblock);
		block_offset = offset % EXT2_BLOCKSIZE_GET(superblock);
	}

	while(1)
	{
		void * addr = ext2_inodeblock_get(block, inode, superblock);

		if(index <= (EXT2_BLOCKSIZE_GET(superblock) - block_offset))
		{
			memcpy(buffer, addr + block_offset, index);
			break;
		}

		memcpy(buffer, addr + block_offset, EXT2_BLOCKSIZE_GET(superblock) - block_offset);
		block++;
		index -= EXT2_BLOCKSIZE_GET(superblock) - block_offset;
		block_offset = 0;
	}

	return size;
}