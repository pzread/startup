#include<std.h>
#include<loader.h>
#include<mm.h>

#define vga_info ((struct vga_info*)VGA_INFO)

extern void init_mm(void);

int test_mm(){
    char *ta,*tb,*tc;
    int i;

    for(i = 1;i < 65536;i++){
	ta = (char*)kmalloc(i);
	memset(ta,1,i);
	kfree(ta);
    }
    for(i = 1;i < 4096;i++){
	ta = (char*)kmalloc(i);
	tb = (char*)kmalloc(4096 - i);
	tc = (char*)kmalloc(i * 2);
	memset(ta,1,i);
	memset(tb,1,4096 - i);
	memset(tc,1,i * 2);
	kfree(ta);
	kfree(tc);
	kfree(tb);
    }

    ta = (char*)kmalloc(0x212345);
    memset(ta,1,0x212345);
    kfree(ta);

    return 0;
}

__attribute__ ((section (".entry")))
void main(void){
    init_mm();
    map_page(vga_info->vmem_base,vga_info->vmem_base);

    log("Enter x86-64 long mode");
    log("Memory Management init");
    log("Video memory map init");

    test_mm();
    log("Memory Management test");

    while(1);
}
