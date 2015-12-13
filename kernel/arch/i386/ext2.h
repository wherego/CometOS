#ifndef _COMETOS_ARCH_I386_EXT2_H
#define _COMETOS_ARCH_I386_EXT2_H

#define EXT2_MAGIC = 0xEF53

#define EXT2_STATE_CLEAN 1
#define EXT2_STATE_ERROR 2

#define EXT2_ERROR_IGNORE 1
#define EXT2_ERROR_REMOUNT 2
#define EXT2_ERROR_PANIC 3

#define EXT2_OSID_LINUX 0
#define EXT2_OSID_GNUGURD 1
#define EXT2_OSID_MASIX 2
#define EXT2_OSID_FREEBSD 3
#define EXT2_OSID_OTHER 4

#define EXT2_BLOCK_SIZE_MIN 1024
#define EXT2_BLOCK_SIZE_MAX 4096
#define EXT2_BLOCK_SIZE_LOG 10

#define EXT2_FRAG_SIZE_MIN 1024
#define EXT2_FRAG_SIZE_MAX 4096
#define EXT2_FRAG_SIZE_LOG 10

#define EXT2_IFSOCK	0xC000
#define EXT2_IFLNK	0xA000
#define EXT2_IFREG	0x8000
#define EXT2_IFBLK	0x6000
#define EXT2_IFDIR	0x4000
#define EXT2_IFCHR	0x2000
#define EXT2_IFIFO	0x1000

#define EXT2_ISUID	0x0800
#define EXT2_ISGID	0x0400
#define EXT2_ISVTX	0x0200

#define EXT2_IRUSR	0x0100
#define EXT2_IWUSR	0x0080
#define EXT2_IXUSR	0x0040
#define EXT2_IRGRP	0x0020
#define EXT2_IWGRP	0x0010
#define EXT2_IXGRP	0x0008
#define EXT2_IROTH	0x0004
#define EXT2_IWOTH	0x0002
#define EXT2_IXOTH	0x0001

enum
{
	EXT2_FILE_UNKNOWN = 0,
	EXT2_FILE_REG_FILE = 1,
	EXT2_FILE_DIR = 2,
	EXT2_FILE_CHRDEV = 3,
	EXT2_FILE_BLKDEV = 4,
	EXT2_FILE_FIFO = 5,
	EXT2_FILE_SOCK = 6,
	EXT2_FILE_SYMLINK = 7
};

struct ext2_superblock
{
	uint32_t inode_total;
	uint32_t block_total;
	uint32_t block_rev_total;
	uint32_t block_free_total;
	uint32_t inode_free_total;
	uint32_t block_superblock_index;
	uint32_t block_size_log;
	uint32_t frag_size_log;
	uint32_t block_per_group;
	uint32_t frag_per_group;
	uint32_t inode_per_group;
	uint32_t mount_time; //Last time in POSIX time
	uint32_t write_time; //Last time in POSIX time
	uint16_t mount_total;
	uint16_t mount_max_total;
	uint16_t magic;
	uint16_t state;
	uint16_t error;
	uint16_t ver_minor;
	uint32_t con_time;
	uint32_t con_interval;
	uint32_t id;
	uint32_t ver_major;
	uint16_t uid;
	uint16_t gid;

	//Extended Superblock - byte 84 & >
	uint32_t inode_first;
	uint16_t inode_size;
	uint16_t block_group;
	uint32_t feature_optional;
	uint32_t feature_required;
	uint32_t feature_nosupport;
	uint8_t uuid[16];
	uint8_t vol_name[16];
	uint8_t mount_last_path[16];
	uint32_t compression;
	uint8_t block_prealloc_file;
	uint8_t block_prealloc_dir;
	uint16_t unused;
	uint8_t journal_id[16];
	uint32_t journal_inode;
	uint32_t journal_dev;
	uint32_t orphan_inode;

	uint32_t hash_seed[4];
	uint8_t hash_ver;
	uint16_t unused2;
	uint8_t unused3;

	uint32_t mount_opt;
	uint32_t meta;
	uint8_t unused4[760];
} __attribute__ ((packed));

typedef struct ext2_superblock ext2_superblock_t;

struct ext2_dir_entry
{
	uint32_t inode;
	uint16_t rec_len;
	uint16_t name_len;
	char name[];
} __attribute__ ((packed));

typedef struct ext2_dir_entry ext2_dir_entry_t;

struct ext2_blockgroup_descriptor
{
	uint32_t block_bitmap_addr;
	uint32_t inode_bitmap_addr;
	uint32_t inode_table_addr;
	uint16_t block_free_total;
	uint16_t inode_free_total;
	uint16_t dir_total;
	uint16_t unused;
	uint8_t rev[12];
} __attribute__ ((packed));

typedef struct ext2_blockgroup_descriptor ext2_bg_descriptor_t;

struct ext2_inode_table
{
	uint16_t type;
	uint16_t uid;
	uint32_t size;
	uint32_t atime;
	uint32_t ctime;
	uint32_t mtime;
	uint32_t dtime;
	uint16_t gid;
	uint16_t link_total;
	uint32_t block_total;
	uint32_t flag;
	uint32_t osid;
	uint32_t pointer[15];
	uint32_t gen;
	uint32_t file_acl;
	uint32_t dir_acl;
	uint32_t frag_addr;
	uint8_t osid_extra[12];
} __attribute__ ((packed));

typedef struct ext2_inode_table ext2_inode_table_t;

ext2_dir_entry_t * ext2_dir_getentry(ext2_inode_table_t * inode, uint32_t index);
ext2_inode_table_t * ext2_inode_get(uint32_t inode, uintptr_t * inode_table,ext2_superblock_t * superblock);
void * ext2_block_get(uint32_t block, uintptr_t * ext2_addr, ext2_superblock_t * superblock);
void * ext2_addr_get(ext2_inode_table_t * inode, uint32_t block, ext2_superblock_t * superblock);

#endif