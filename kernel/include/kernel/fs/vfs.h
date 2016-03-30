#ifndef _COMETOS_ARCH_I386_VFS_H
#define _COMETOS_ARCH_I386_VFS_H

#include <stdint.h>
#include <string.h>

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

struct file;

typedef uint32_t (*file_read_t)(struct file*,uint32_t,uint32_t,uint8_t*);
typedef uint32_t (*file_write_t)(struct file*,uint32_t,uint32_t,uint8_t*);
typedef void (*file_open_t)(struct file*);
typedef void (*file_close_t)(struct file*);
typedef struct dir_entry * (*file_readdir_t)(struct file*,uint32_t);
typedef struct file * (*file_finddir_t)(struct file*,char *name);

typedef struct file
{
    char name[128];     // The filename.
    uint32_t mask;        // The permissions mask.
    uint32_t uid;         // The owning user.
    uint32_t gid;         // The owning group.
    uint32_t flag;       // Includes the node type. See #defines above.
    uint32_t inode;       // This is device-specific - provides a way for a filesystem to identify files.
    uint32_t size;      // Size of the file, in bytes.
    uint32_t impl;        // An implementation-defined number.
    file_read_t read;
    file_write_t write;
    file_open_t open;
    file_close_t close;
    file_readdir_t readdir;
    file_finddir_t finddir;
    struct file *ptr; // Used by mountpoints and symlinks.
} file_t;

struct dir_entry
{
    char name[128]; // Filename.
    uint32_t inode;     // inode number. Required by POSIX.
};

file_t *fs_root; // The root of the filesystem.

uint32_t vfs_read(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t vfs_write(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void vfs_open(file_t *node, uint8_t read, uint8_t write);
void vfs_close(file_t *node);
struct dir_entry * vfs_readdir(file_t *node, uint32_t index);
file_t * vfs_finddir(file_t *node, char *name);
char * vfs_path(char *cwd, char *input);

#endif