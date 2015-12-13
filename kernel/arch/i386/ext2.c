#include <stdint.h>
#include <string.h>

#include "vfs.h"
#include "ext2.h"
#include "../arch/i386/liballoc.h"

void * ext2_block_get(uint32_t block, uintptr_t * ext2_addr, ext2_superblock_t * superblock)
{
	return (void *)((uintptr_t)ext2_addr + (EXT2_BLOCK_SIZE_MIN << superblock->block_size_log) * block);
}

ext2_inode_table_t * ext2_inode_get(uint32_t inode, uintptr_t * inode_table, ext2_superblock_t * superblock)
{
	return (ext2_inode_table_t *)((uintptr_t)inode_table + superblock->inode_size * (inode - 1));
}