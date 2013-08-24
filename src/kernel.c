#include<std.h>
#include<loader.h>
#include<mm.h>

#define vga_info ((struct vga_info*)VGA_INFO)

extern void init_mm(void);
extern void init_acpi(void);

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

static unsigned long args[3];
static char str[64];
void main(void){
    init_mm();
    map_page(vga_info->vmem_base,vga_info->vmem_base);
    log("Enter x86-64 long mode");
    log("Memory Management init");
    log("Video memory map init");

    test_mm();
    log("Memtest done");

    init_acpi();

    log("Hello UEFI");
    
    int i;
    struct mem_info *mem_info;
    unsigned long base;
    unsigned long size;
    
    mem_info = (struct mem_info*)MEM_INFO;
    for(i = 0;i < mem_info->region_count;i++){
	if(mem_info->region[i].type == 1){
	    base = mem_info->region[i].base;
	    size = mem_info->region[i].size;

	    args[0] = base;
	    args[1] = size;
	    sprintf(str,"%x %x",args);
	    log(str);
	}
    }

    while(1);
}


