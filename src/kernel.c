#include<std.h>
#include<loader.h>
#include<mm.h>

#define vga_info ((struct vga_info*)VGA_INFO)

extern void init_mm(void);

__attribute__ ((section (".text")))
void main(void){
    int i;
    struct mem_info *mem_info;

    init_mm();
    map_page(vga_info->vmem_base,vga_info->vmem_base);

    log("Enter x86-64 long mode");
    log("Init memory management done");
    log("Video memory map done");

    while(1);
}
