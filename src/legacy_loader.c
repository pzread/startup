__asm__(".code16gcc\n");

#define LOADER

#include <loader.h>

#define GET_WORDPTR(p,v) __asm__ __volatile__("push es\nmov edi,%1\nand edi,0xFFFF0000\nshr edi,4\nmov es,di\nmov edi,%1\nmov %0,WORD PTR es:[di]\npop es\n":"=g"(v):"g"(p):"edi")
#define SET_DWORDPTR(p,v) __asm__ __volatile__("push es\nmov edi,%0\nand edi,0xFFFF0000\nshr edi,4\nmov es,di\nmov edi,%0\nmov DWORD PTR es:[di],%1\npop es\n"::"g"(p),"g"(v):"edi")

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
    char pad_a[14];
    unsigned short bytes_per_scanline;
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

static void init_memory(void){
    unsigned int next_entry;
    __volatile__ struct mem_info *mem_info;

    next_entry = 0;
    mem_info = (struct mem_info*)MEM_INFO;
    mem_info->region_count = 0;
    do{
	mem_info->region[mem_info->region_count].acpi = 1;

	__asm__ __volatile__(
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
    __volatile__ struct vbe_info_block vbe_info;
    unsigned short *mode_ptr;
    unsigned short mode;
    __volatile__ struct mode_info_block mode_info;
    __volatile__ struct vga_info *vga_info;

    __asm__ __volatile__(
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

	__asm__ __volatile__(
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
	__asm__ __volatile__(
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
    vga_info->bytes_per_scanline = mode_info.bytes_per_scanline;
}
static void init_kernel(){
    __volatile__ struct disk_packet diskpack = {
	.size_of_packet = sizeof(struct disk_packet),
	.reserved = 0,
	.sectors = 64,
	.offset = 0xF000,
	.segment = 0,
	.lba = 5,
	.flat = 0
    };
    
    __asm__ __volatile__(
	"mov esi,%0\n"
	"mov ah,0x42\n"
	"mov dl,0x80\n"
	"int 0x13\n"
    ::"g"(&diskpack):"eax","edx","esi");
}
static void prepare_kernel(){
    int i;
    int j;

    __volatile__ unsigned int *intermap;
    __volatile__ unsigned int *iomap;
    __volatile__ unsigned int *tss;
    unsigned short iomap_off;

    __volatile__ unsigned long long *gdt;
    __volatile__ struct gdt_ptr gdt_ptr;

    __volatile__ unsigned int *pml;
    __volatile__ unsigned int *pdpte;
    __volatile__ unsigned int *pde;
    
    intermap = (unsigned int*)(IOMAP_BASE - 32);
    for(i = 0;i < 8;i++){
	intermap[i] = 0;
    }

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
	tss[25] = iomap_off << 16;

	tss += 26;  //4 * 26 = 104
	iomap_off -= 104;
    }
    
    gdt = (unsigned long long*)GDT_BASE;
    gdt[0] = 0;
    gdt[1] = 0x20980000000000;
    gdt[2] = 0x900000000000;

    for(i = 0,j = 3;i < MAX_PROCESSOR;i++,j += 2){
	gdt[j] = 0x89000000D5FF | ((TSS_BASE + i * 104) << 16);
	gdt[j + 1] = 0;
    }

    pml = (unsigned int*)0x7D000;
    pdpte = (unsigned int*)0x7E000;
    pde = (unsigned int*)0x7F000;
    for(i = 0;i < 1024;i++){
	SET_DWORDPTR(pml + i,0);
	SET_DWORDPTR(pdpte + i,0);
	SET_DWORDPTR(pde + i,0);
    }
    SET_DWORDPTR(pde,0x0 | 0x83);   //Init low 2M memory
    SET_DWORDPTR(pdpte,0x7F000 | 0x3);
    SET_DWORDPTR(pml,0x7E000 | 0x3);
    
    gdt_ptr.limit = 24 + 16 * MAX_PROCESSOR - 1;
    gdt_ptr.base = GDT_BASE;

    __asm__ __volatile__(
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
    prepare_kernel();

    __asm__ __volatile__(
	"jmp %0:0xF000\n"   //Start kernel
    ::"i"(GDT_CODE):);
}
