asm(".code16gcc\n");
#define LOADER

#include <loader.h>

#define GET_WORDPTR(p,v) asm volatile("push es\nmov edi,%1\nand edi,0xFFFF0000\nshr edi,4\nmov es,di\nmov edi,%1\nmov %0,WORD PTR es:[di]\npop es\n":"=g"(v):"g"(p):"edi")
#define SET_DWORDPTR(p,v) asm volatile("push es\nmov edi,%0\nand edi,0xFFFF0000\nshr edi,4\nmov es,di\nmov edi,%0\nmov DWORD PTR es:[di],%1\npop es\n"::"g"(p),"g"(v):"edi")

#pragma pack(1)
struct vbe_info_block{
    char signature[4];
    unsigned short version;
    void *oem_string_ptr;
    unsigned char capabilities[4];
    void *mode_ptr;
    unsigned short total_memory; 
    unsigned short oem_software_rev;
    void *oem_vendor_name_ptr;
    void *oem_product_name_ptr;
    void *oem_product_rev_ptr;
    unsigned char reserved[222];
    unsigned char oem_data[256];
};
struct mode_info_block{
    unsigned short mode_attributes;
    char pad_a[16];
    unsigned short x_resolution;
    unsigned short y_resolution;
    char pad_b[3];
    unsigned char bits_per_pixel;
    char pad_c[14];
    void *phys_base_ptr;
    unsigned char reserved[212];
};

struct disk_packet{
    unsigned char size_of_packet;
    unsigned char reserved;
    unsigned short sectors;
    unsigned short offset;
    unsigned short segment;
    unsigned long long lba;
    unsigned long long flat;
};

struct gdt_ptr{
    unsigned short limit;
    unsigned long long base;
};

/*static char *prefix = "[startup loader] ";

static void puts(char *str){
    int i;

    i = 0;
    while(str[i] != '\0'){
	asm(
	    "mov ah,0xE\n"
	    "mov al,%0\n"
	    "int 0x10\n"
	::"g"(str[i]):"eax");

	i++;
    }
}
static void log(char *msg){
    puts(prefix);
    puts(msg);
    puts("\r\n");
}*/

static void init_memory(void){
    unsigned int next_entry;
    volatile struct mem_info *mem_info;

    next_entry = 0;
    mem_info = (struct mem_info*)MEM_INFO;
    mem_info->region_count = 0;
    do{
	mem_info->region[mem_info->region_count].acpi = 1;

	asm volatile(
	    "mov edi,%2\n"
	    "mov eax,0xE820\n"	    
	    "mov ebx,%0\n"
	    "mov ecx,24\n"
	    "mov edx,0x534D4150\n"
	    "int 0x15\n"
	    "mov %0,ebx\n"
	:"=g"(next_entry)
	:"0"(next_entry),"g"(&mem_info->region[mem_info->region_count])
	:"eax","ebx","ecx","edx","edi","memory");

	mem_info->region_count += 1;
    }while(next_entry != 0);
}
static void init_video(void){
    struct vbe_info_block vbe_info;
    unsigned short *mode_ptr;
    unsigned short mode;
    volatile struct mode_info_block mode_info;
    volatile struct vga_info *vga_info;

    asm volatile(
	"mov ax,0x4F00\n"
	"mov edi,%0\n"
	"int 0x10\n"
    ::"g"(&vbe_info):"eax","edi","memory");
    
    mode_ptr = vbe_info.mode_ptr;
    while(1){
	GET_WORDPTR(mode_ptr,mode);
	if(mode == 0xFFFF){
	    break;
	}

	asm volatile(
	    "mov ax,0x4F01\n"
	    "mov cx,%0\n"
	    "mov edi,%1\n"
	    "int 0x10\n"
	::"g"(mode),"g"(&mode_info):"eax","ecx","edi","memory");
	
	if((mode_info.mode_attributes & 0x90) == 0x90 &&    //Graphics mode | Linear frame buffer mode
	    mode_info.x_resolution >= 800 &&
	    mode_info.y_resolution >= 600 &&
	    mode_info.bits_per_pixel == 24){

	    break;
	}

	mode_ptr++;
    }

    if(mode != 0xFFFF){
	asm volatile(
	    "mov ax,0x4F02\n"
	    "mov ebx,%0\n"
	    "int 0x10\n"
	::"g"(mode | 0x4000):"eax","ebx");

    }else{
	while(1);
    }

    vga_info = (struct vga_info*)VGA_INFO;
    vga_info->vmem_base = (unsigned int)mode_info.phys_base_ptr;
    vga_info->x_res = mode_info.x_resolution;
    vga_info->y_res = mode_info.y_resolution;
    vga_info->bits = mode_info.bits_per_pixel;

    asm volatile(
	"push ebp\n"
	"push ds\n"
	"push es\n"
	"mov ax,0x1130\n"
	"mov bh,0x6\n"
	"int 0x10\n"
	"push es\n"
	"pop ds\n"
	"pop es\n"
	"mov si,bp\n"
	"mov di,%0\n"
	"mov ecx,256 * 16 / 4\n"
	"rep movsd\n"
	"pop ds\n"
	"pop ebp\n"
    ::"i"(VFONT_BASE):"eax","ecx","edx","edi","esi","memory");
}
static void init_kernel(){
    volatile struct disk_packet diskpack = {
	.size_of_packet = sizeof(struct disk_packet),
	.reserved = 0,
	.sectors = 32,
	.offset = 0,
	.segment = 0x1000,
	.lba = 5,
	.flat = 0
    };
    
    asm volatile(
	"mov esi,%0\n"
	"mov ah,0x42\n"
	"mov dl,0x80\n"
	"int 0x13\n"
    ::"g"(&diskpack):"eax","edx","esi");
}
static void enter_long_mode(){
    int i;
    int j;

    unsigned int *iomap;
    unsigned int *tss;
    unsigned short iomap_off;

    unsigned long long *gdt;

    unsigned int *pml;
    unsigned int *pdpte;
    unsigned int *pde;

    volatile struct gdt_ptr gdt_ptr;

    iomap = (unsigned int*)IOMAP_BASE;
    for(i = 0;i < 2048;i++){
	iomap[i] = 0;
    }
    ((unsigned char*)(IOMAP_BASE))[8192] = 0xFF;

    tss = (unsigned int*)TSS_BASE;
    iomap_off = IOMAP_BASE - TSS_BASE;
    for(i = 0;i < MAX_PROCESSOR;i++){
	for(j = 0;j < 25;j++){
	    tss[j] = 0;
	}	
	tss[25] = iomap_off;

	tss += 26;  //4 * 26 = 104
	iomap_off -= 104;
    }
    
    gdt = (unsigned long long*)GDT_BASE;
    gdt[0] = 0;
    gdt[1] = 0x20980000000000;
    gdt[2] = 0x900000000000;

    for(i = 0,j = 3;i < MAX_PROCESSOR;i++,j += 2){
	gdt[j] = 0x890000000067 | (TSS_BASE + i * 104);
	gdt[j + 1] = 0;
    }

    pml = (unsigned int*)0x7B000;
    pdpte = (unsigned int*)0x7C000;
    pde = (unsigned int*)0x7D000;
    for(i = 0;i < 1024;i++){
	SET_DWORDPTR(pml + i,0);
	SET_DWORDPTR(pdpte + i,0);
	SET_DWORDPTR(pde + i,0);
    }
    SET_DWORDPTR(pml,0x7C000 | 0x3);
    SET_DWORDPTR(pdpte,0x7D000 | 0x3);
    SET_DWORDPTR(pde,0x0 | 0x83);   //Init base ram
    
    pdpte = (unsigned int*)0x7E000;
    pde = (unsigned int*)0x7F000;
    for(i = 0;i < 1024;i++){
	SET_DWORDPTR(pdpte + i,0);
	SET_DWORDPTR(pde + i,0);
    }
    SET_DWORDPTR(pml + 256 * 2,0x7E000 | 0x3);
    SET_DWORDPTR(pdpte,0x7F000 | 0x3);
    SET_DWORDPTR(pde,0x0 | 0x83);   //Init 4MB ram
    SET_DWORDPTR(pde + 2,0x200000 | 0x83);

    gdt_ptr.limit = 24 + 16 * MAX_PROCESSOR - 1;
    gdt_ptr.base = GDT_BASE;

    asm volatile(
	"mov eax,%0\n"	//Load page table
	"mov cr3,eax\n"
	"mov eax,cr4\n"	//Enable PAE
	"or eax,0x20\n"
	"mov cr4,eax\n"	
	"mov ecx,0xC0000080\n"	//Enable LME
	"rdmsr\n"
	"or eax,0x100\n"
	"wrmsr\n"
	"cli\n"
	"mov eax,cr0\n"	//Enable long mode
	"or eax,0x80000001\n"
	"mov cr0,eax\n"
	"lgdt [%1]\n"	//Load GDT
	"mov eax,%2\n"	//Load TSS
	"ltr ax\n"
	"mov ax,%3\n"	//Init fs,gs
	"mov fs,ax\n"
	"mov gs,ax\n"
    ::"g"(pml),"g"(&gdt_ptr),"i"(GDT_TSSD_BASE),"i"(GDT_DATA):"eax","ecx","edx");
}

__attribute__ ((section (".text")))
void main(void){
    init_memory();
    init_video();
    init_kernel();
    enter_long_mode();

    asm volatile(
	"jmp %0:0x7DC0\n"   //Start kernel
    ::"i"(GDT_CODE):);
}
