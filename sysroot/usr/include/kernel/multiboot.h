#ifndef _KERNEL_MULTIBOOT_H
#define _KERNEL_MULTIBOOT_H

#define MULTIBOOT_FLAG_MEM 0x001
#define MULTIBOOT_FLAG_DEVICE 0x002
#define MULTIBOOT_FLAG_CMDLINE 0x004
#define MULTIBOOT_FLAG_MODS 0x008
#define MULTIBOOT_FLAG_AOUT 0x010
#define MULTIBOOT_FLAG_ELF 0x020
#define MULTIBOOT_FLAG_MMAP 0x040
#define MULTIBOOT_FLAG_CONFIG 0x080
#define MULTIBOOT_FLAG_LOADER 0x100
#define MULTIBOOT_FLAG_APM 0x200
#define MULTIBOOT_FLAG_VBE 0x400

struct multiboot
{
uintptr_t flags;
uintptr_t mem_lower;
uintptr_t mem_upper;
uintptr_t boot_device;
uintptr_t cmdline;
uintptr_t mods_count;
uintptr_t mods_addr;
uintptr_t num;
uintptr_t size;
uintptr_t addr;
uintptr_t shndx;
uintptr_t mmap_length;
uintptr_t mmap_addr;
uintptr_t drives_length;
uintptr_t drives_addr;
uintptr_t config_table;
uintptr_t boot_loader_name;
uintptr_t apm_table;
uintptr_t vbe_control_info;
uintptr_t vbe_mode_info;
uintptr_t vbe_mode;
uintptr_t vbe_interface_seg;
uintptr_t vbe_interface_off;
uintptr_t vbe_interface_len;
} __attribute__((packed));

uint32_t module_start;

__attribute__((unused))
static inline void multiboot_print(struct multiboot *mboot_ptr)
{
	printf("MULTIBOOT header at 0x%x:\n", (uintptr_t)mboot_ptr);
	printf("Flags : 0x%x ", mboot_ptr->flags);
	printf("Mem Lo: 0x%x ", mboot_ptr->mem_lower);
	printf("Mem Hi: 0x%x ", mboot_ptr->mem_upper);
	printf("Boot d: 0x%x\n", mboot_ptr->boot_device);
	printf("cmdlin: 0x%x ", mboot_ptr->cmdline);
	printf("Mods : 0x%x ", mboot_ptr->mods_count);
	printf("Addr : 0x%x ", mboot_ptr->mods_addr);
	printf("Syms : 0x%x\n", mboot_ptr->num);
	printf("Syms : 0x%x ", mboot_ptr->size);
	printf("Syms : 0x%x ", mboot_ptr->addr);
	printf("Syms : 0x%x ", mboot_ptr->shndx);
	printf("MMap : 0x%x\n", mboot_ptr->mmap_length);
	printf("Addr : 0x%x ", mboot_ptr->mmap_addr);
	printf("Drives: 0x%x ", mboot_ptr->drives_length);
	printf("Addr : 0x%x ", mboot_ptr->drives_addr);
	printf("Config: 0x%x\n", mboot_ptr->config_table);
	printf("Loader: 0x%x ", mboot_ptr->boot_loader_name);
	printf("APM : 0x%x ", mboot_ptr->apm_table);
	printf("VBE Co: 0x%x ", mboot_ptr->vbe_control_info);
	printf("VBE Mo: 0x%x\n", mboot_ptr->vbe_mode_info);
	printf("VBE In: 0x%x ", mboot_ptr->vbe_mode);
	printf("VBE se: 0x%x ", mboot_ptr->vbe_interface_seg);
	printf("VBE of: 0x%x ", mboot_ptr->vbe_interface_off);
	printf("VBE le: 0x%x\n", mboot_ptr->vbe_interface_len);

	if (mboot_ptr->flags & (1 << 2))
	{
		printf("Started with: %s\n", (char *)mboot_ptr->cmdline);
	}

	if (mboot_ptr->flags & (1 << 9))
	{
		printf("Booted from: %s\n", (char *)mboot_ptr->boot_loader_name);
	}

	if (mboot_ptr->flags & (1 << 0))
	{
		printf("%dkB lower memory\n", mboot_ptr->mem_lower);
		printf("%dkB higher memory ", mboot_ptr->mem_upper);
		int mem_mb = mboot_ptr->mem_upper / 1024;
		printf("(%dMB)\n", mem_mb);
	}

	if (mboot_ptr->flags & (1 << 3))
	{
		printf("Found %d module(s).\n", mboot_ptr->mods_count);
		if (mboot_ptr->mods_count > 0)
		{
			uint32_t i;
			for (i = 0; i < mboot_ptr->mods_count; ++i )
			{
				module_start = *((uint32_t*)mboot_ptr->mods_addr + 8 * i);
				uint32_t module_end = *(uint32_t*)(mboot_ptr->mods_addr + 8 * i + 4);
				ponter_move_int(module_end - module_start, 0, 0);
				printf("Module %d is at 0x%x:0x%x\n", i+1, module_start, module_end);
			}
		}
	}
}

#endif