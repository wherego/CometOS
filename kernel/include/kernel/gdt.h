#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

typedef unsigned int u32int;
typedef signed int s32int;
typedef unsigned short u16int;
typedef signed short s16int;
typedef unsigned char u8int;
typedef signed char s8int;

typedef struct gdt_descr
{
	u16int limit_low;
	u16int base_low;
	u8int base_middle;
	u8int access;
	u8int granularity;
	u8int base_high;
} __attribute__((packed)) gdt_descr_t;

typedef struct gdt_ptr
{
	u16int limit;
	u32int base;
} __attribute__((packed)) gdt_ptr_t;

void gdt_initialize();

static void gdt_set_desc(gdt_descr_t* descr, u32int base, u32int limit, u8int access, u8int granularity);

static void gdt_set(gdt_ptr_t* gdt_ptr);

#endif
