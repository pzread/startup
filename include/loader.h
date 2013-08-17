/*
Memory Map
   
Physical Memory
0x500	    -	    0x50F	VGA Info
0x510	    -	    0xB10	MEM Info
0x1000	    -	    0x77FF	TSS (104 * 256)
0x78C0	    -	    0x85FF	Loader (2KB stack + 2KB loader)
0x8600	    -	    0xA5FF	Descriptor table (8 * 1024)
0xA600	    -	    0xB5FF	ISR (16 * 256)
0xB600	    -	    0xC5FF	VGA Font
0xC600	    -	    0xE600	IO bitmap (64Kb + 1B)
0x10000	    -	    0x13FFF	Kernel (16KB)
0x7B000	    -	    0x7FFFF	Loader page table

Loader Init Map
0x0	    -	    0x200000	=>  0x0			-   0x200000    		
0x0	    -	    0x200000	=>  0xFFFF800000000000  -   0xFFFF800000200000    		
0x200000    -	    0x400000	=>  0xFFFF800000200000  -   0xFFFF800000400000    		

Kernel Entry
0xFFFF800000010000

*/

#ifdef LOADER

#define HIGH_OFFSET 0

#else

#define HIGH_OFFSET 0xFFFF800000000000UL

#endif

#define MAX_PROCESSOR 256
#define VGA_INFO (0x500 + HIGH_OFFSET)
#define MEM_INFO (0x510 + HIGH_OFFSET)
#define TSS_BASE (0x1000 + HIGH_OFFSET)
#define GDT_BASE (0x8600 + HIGH_OFFSET)
#define VFONT_BASE (0xB600 + HIGH_OFFSET)
#define IOMAP_BASE (0xC600 + HIGH_OFFSET)

#define GDT_CODE 0x8
#define GDT_DATA 0x10
#define GDT_TSSD_BASE 0x18

#pragma pack(push)
#pragma pack(1)

struct vga_info{
    unsigned int vmem_base;
    unsigned int x_res;
    unsigned int y_res;
    unsigned int bits;
};
struct mem_region{
    unsigned long long base;
    unsigned long long size;
    unsigned int type;
    unsigned int acpi;
};
struct mem_info{
    unsigned char region_count;
    struct mem_region region[64];
};

#pragma pack(pop)
