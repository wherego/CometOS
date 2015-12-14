#include <stdint.h>
#include <string.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/ext2.h>

void * ext2_block_get(uint32_t block, uintptr_t * ext2_addr, ext2_superblock_t * superblock)
{
	return (void *)((uintptr_t)ext2_addr + (EXT2_BLOCK_SIZE_MIN << superblock->block_size_log) * block);
}

ext2_inode_table_t * ext2_inode_get(uint32_t inode, uintptr_t * inode_table, ext2_superblock_t * superblock)
{
	return (ext2_inode_table_t *)((uintptr_t)inode_table + superblock->inode_size * (inode - 1));
}

int ext2_superblock_validate(ext2_superblock_t * superblock)
{
	if(superblock->magic == EXT2_MAGIC)
		return 1;
	else
		return 0;
}

int ext2_state_get(ext2_superblock_t * superblock)
{
	if(superblock->state == EXT2_STATE_CLEAN)
		return EXT2_STATE_CLEAN;
	else if(superblock->state == EXT2_STATE_ERROR)
		return EXT2_STATE_ERROR;

	return 0;
}

char ext2_osid_get(ext2_superblock_t * superblock)
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

void ext2_error(ext2_superblock_t * superblock)
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
		//Panic ...
		break;
	}
}

int ext2_error_get(ext2_superblock_t * superblock)
{
	switch(superblock->error)
	{
		case EXT2_ERROR_IGNORE:
			return EXT2_ERROR_IGNORE; //Ignore
		break;

		case EXT2_ERROR_REMOUNT:
			return EXT2_ERROR_REMOUNT; //Remount
		break;

		case EXT2_ERROR_PANIC:
			return EXT2_ERROR_PANIC; //Panic ...
		break;
	}

	return 0;
}