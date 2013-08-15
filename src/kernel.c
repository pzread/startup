#include<std.h>
#include<loader.h>
#include<mm.h>

#define vga_info ((struct vga_info*)VGA_INFO)

extern void init_mm(void);

__attribute__ ((section (".text")))
void main(void){
    char *test;
    int i;

    init_mm();
    map_page(vga_info->vmem_base,vga_info->vmem_base);

    log("Enter x86-64 long mode");
    log("Init memory management done");
    log("Video memory map done");

    for(i = 0;i < 65536;i++){
	test = (char*)kmalloc(i);
	memset(test,1,i);
	//log(test);
    }
    test = (char*)kmalloc(0x300000);
    memset(test,1,0x300000);
    log("Test kmem passed");

    while(1);
}
