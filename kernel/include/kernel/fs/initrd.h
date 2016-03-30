#ifndef _COMETOS_ARCH_I386_INITRD_H
#define _COMETOS_ARCH_I386_INITRD_H

#include "vfs.h"
#include "ext2.h"

file_t * initrd_root;
ext2_t * ext2_initrd;

void initrd_initialize(void * addr);
uint32_t initrd_read(file_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
struct dir_entry * initrd_readdir(file_t *node, uint32_t index);
file_t *initrd_finddir(file_t *node, char *name);

#endif