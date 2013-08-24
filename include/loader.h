/*

Physical Memory
0x500	    -	    0x513	VGA Info
0x514	    -	    0xB14	MEM Info
0xB15	    -	    0xB38	RSDP Info
0x1000	    -	    0x77FF	TSS (104 * 256)
0x78C0	    -	    0x85FF	Loader (2KB stack + 2KB loader)
0x8600	    -	    0xA5FF	Descriptor table (8 * 1024)
0xA600	    -	    0xB5FF	ISR (16 * 256)
0xC5E0	    -	    0xC5FF	Interrupt redirection bitmap
0xC600	    -	    0xE600	IO bitmap (64Kb + 1B)
0xF000	    -	    0x17000	Kernel (32KB)
0x7D000	    -	    0x7FFFF	Temporarily page table

Legacy Loader Init Map
0x0	    -	    0x200000	=>  0x0		-	0x200000    		

*/

#ifdef LOADER

#define HIGH_OFFSET 0

#else

#define HIGH_OFFSET 0xFFFF800000000000UL

#endif

#define MAX_PROCESSOR 256
#define VGA_INFO (0x500 + HIGH_OFFSET)
#define MEM_INFO (0x514 + HIGH_OFFSET)
#define RSDP_INFO (0xB15 + HIGH_OFFSET)
#define TSS_BASE (0x1000 + HIGH_OFFSET)
#define GDT_BASE (0x8600 + HIGH_OFFSET)
#define IOMAP_BASE (0xC600 + HIGH_OFFSET)

#define GDT_CODE 0x8
#define GDT_DATA 0x10
#define GDT_TSSD_BASE 0x18

#define MEM_TYPE_USABLE 0x1
#define MEM_TYPE_RESERVED 0x2

#pragma pack(push)
#pragma pack(1)

struct vga_info{
    unsigned int vmem_base;
    unsigned int x_res;
    unsigned int y_res;
    unsigned int bits;
    unsigned int bytes_per_scanline;
};
struct mem_region{
    unsigned long long base;
    unsigned long long size;
    unsigned int type;
    unsigned int acpi;	//For legacy loader
};
struct mem_info{
    unsigned char region_count;
    struct mem_region region[64];
};
struct rsdp{
    unsigned long signature;
    unsigned char checksum;
    unsigned char oemid[6];
    unsigned char revision;
    unsigned int rsdt_base;
    unsigned int length;
    unsigned long xsdt_base;
    unsigned char ext_checksum;
    unsigned char reserved[3];
};

#pragma pack(pop)
