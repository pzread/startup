#define NULL ((void*)0)
#define PAGE_SIZE 0x200000UL
#define PAGE_SHIFT 21UL

#define KERNEL_AREA_START 0xFFFF800000000000UL
#define KERNEL_HEAP_START 0xFFFF800000400000UL
#define KERNEL_HEAP_END 0xFFFF900000000000UL
#define KERNEL_MAP_START 0xFFFFB00000000000UL

#define KERNEL_MAP_ACPI 0xFFFFB00000000000UL

extern int map_page(unsigned long dst,unsigned long src);
extern int map_pages(unsigned long dst,unsigned long src,unsigned long size);
extern void* kmalloc(unsigned long size);
extern void kfree(void *ptr);
