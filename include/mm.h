#define NULL ((void*)0)
#define PAGE_SIZE 0x200000UL
#define PAGE_SHIFT 21UL

#define KERNEL_AREA_START 0xFFFF800000000000UL
#define KERNEL_AREA_END 0x9000000000000000UL
#define KSTACK_AREA_START 0x9000000000000000UL
#define KSTACK_AREA_END 0xA000000000000000UL

extern int map_page(unsigned long dst,unsigned long src);
extern void* kmalloc(unsigned long size);
extern void kfree(void *ptr);
