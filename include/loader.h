/*
Memory Map
   
0x500	    -	    0x510	VGA Info
0x1000	    -	    0x77FF	TSS (104 * 256)
0x7900	    -	    0x85FF	Loader (2KB stack + 2KB loader)
0x8600	    -	    0xA5FF	Descriptor table (8 * 1024)
0xA600	    -	    0xB5FF	ISR (16 * 256)
0xB600	    -	    0xC5FF	VGA Font
0xC600	    -	    0xE600	IO bitmap (64Kb + 1B)
0xF000	    -	    0x12FFF	Kernel (16KB)
0x7D000	    -	    0x7FFFF	Loader page table

0x100000    -	    
*/

#define MAX_PROCESSOR 256
#define VGA_INFO 0x500
#define TSS_BASE 0x1000
#define GDT_BASE 0x8600
#define VFONT_BASE 0xB600
#define IOMAP_BASE 0xC600

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

#pragma pack(pop)
