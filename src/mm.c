#include <loader.h>
#include <std.h>

#define NULL ((void*)0)
#define ENOMEM 1
#define PAGE_SIZE 0x200000UL
#define PAGE_SHIFT 21UL

#define PT_PAGEMAP 0x1E8000UL
#define PT_PAGEMAP_SIZE 32768

#define PT_TABLEMAP 0x1F0000UL
#define PT_TABLEMAP_SIZE 65536
#define	PT_BASE 0x200000UL
#define PT_MAX_TABLE 512UL
#define PT_TABLE_SIZE 4096UL

#define PT_FLAG_P 0x1UL
#define PT_FLAG_RW 0x2UL
#define PT_FLAG_US 0x4UL
#define PT_FLAG_PS 0x80
#define PT_FLAG_G 0x100UL
#define PT_FLAG_XD 0x8000000000000000UL

#define PT_PML_TABLE ((unsigned long*)PT_BASE)
#define PT_SET_ENTRY(e,x,f) (e) = (unsigned long)(x) | (unsigned long)((f) | PT_FLAG_P)

static unsigned long table_idx_next = 0;

void memset(void *dst,char value,unsigned long size){
    int i;
    unsigned long long_value;

    long_value = 0;
    for(i = 0;i < sizeof(unsigned long);i++){
	long_value = long_value << sizeof(unsigned char);
	long_value += (unsigned long)value;
    }
    while(size > sizeof(unsigned long)){
	*(unsigned long*)dst = long_value;	
	dst = ((unsigned long*)dst) + 1;
	size -= sizeof(unsigned long);
    }
    while(size > 0){
	*(unsigned char*)dst = value;
	dst = ((unsigned char*)dst) + 1;
	size -= sizeof(unsigned char);
    }
}
int test_bit(void *dst,unsigned long off){
    if(((unsigned char*)dst)[off >> 3UL] & (1 << (off & 7UL))){
	return 1;
    }else{
	return 0;
    }
}
void set_bit(void *dst,unsigned long off){
    ((unsigned char*)dst)[off >> 3UL] &= (1 << (off & 7UL));
}
void clear_bit(void *dst,unsigned long off){
    ((unsigned char*)dst)[off >> 3UL] &= (~(1 << (off & 7UL)));
}

void init_page(void){
    int i;

    struct mem_info *mem_info;
    unsigned long base;
    unsigned long size;
    unsigned long off;
    int idx;
    
    memset((void*)PT_PAGEMAP,-1,PT_PAGEMAP_SIZE);

    mem_info = (struct mem_info*)MEM_INFO;
    for(i = 0;i < mem_info->region_count;i++){
	if(mem_info->region[i].type == 1){
	    base = mem_info->region[i].base;
	    size = mem_info->region[i].size;
	    off = base & (PAGE_SIZE - 1);
	    if(off > 0){
		if(size < off){
		    continue;
		}
		base += (PAGE_SIZE - off);
		size -= off;
	    }

	    idx = base >> PAGE_SHIFT;
	    while(size >= PAGE_SIZE){
		clear_bit((void*)PT_PAGEMAP,idx);

		idx += 1;
		size -= PAGE_SIZE;
	    }
	}
    }
}
void init_table(void){
    memset((void*)PT_TABLEMAP,0,PT_TABLEMAP_SIZE);
    table_idx_next = 0;
}
static unsigned long* alloc_table(void){
    unsigned long *table = NULL;
    int rescan = 0;

    while(1){
	if(table_idx_next == PT_MAX_TABLE){
	    if(rescan == 1){
		break; 
	    }else{
		table_idx_next = 0;
		rescan = 1;
	    }
	}

	if(!test_bit((void*)PT_TABLEMAP,table_idx_next)){
	    table = (unsigned long*)(PT_BASE + table_idx_next * PT_TABLE_SIZE);
	    set_bit((void*)PT_TABLEMAP,table_idx_next);

	    memset(table,0,PT_TABLE_SIZE);
	    
	    table_idx_next += 1;
	    break;
	}

	break;
	
	table_idx_next += 1;
    }

    return table;
}
static void free_table(unsigned long *table){
    unsigned long idx;

    idx = ((unsigned long)table - PT_BASE) / PT_TABLE_SIZE;
    clear_bit((void*)PT_TABLEMAP,idx);
}
int map_page(unsigned long dst,unsigned long src){
    unsigned short pml_idx;
    unsigned short pdpte_idx;
    unsigned short pde_idx;
    unsigned long entry;
    unsigned long *pdpte;
    unsigned long *pde;

    pml_idx = (dst & 0xFF8000000000UL) >> 39UL;
    pdpte_idx = (dst & 0x7FC0000000UL) >> 30UL;
    pde_idx = (dst & 0x3FE00000UL) >> 21UL;

    entry = PT_PML_TABLE[pml_idx];
    if(!(entry & PT_FLAG_P)){
	if((pdpte = alloc_table()) == NULL){
	    return -ENOMEM;
	}
	PT_SET_ENTRY(entry,pdpte,PT_FLAG_RW);
	PT_PML_TABLE[pml_idx] = entry;
    }else{
	pdpte = (unsigned long*)(entry & 0xFFFFFFFFF000);
    }
    
    entry = pdpte[pdpte_idx];
    if(!(entry & PT_FLAG_P)){
	if((pde = alloc_table()) == NULL){
	    return -ENOMEM;
	}
	PT_SET_ENTRY(entry,pde,PT_FLAG_RW);
	pdpte[pdpte_idx] = entry;
    }else{
	pde = (unsigned long*)(entry & 0xFFFFFFFFF000);
    }
    
    PT_SET_ENTRY(pde[pde_idx],src & (~0xFFF),PT_FLAG_PS | PT_FLAG_RW);

    return 0;
}

#define vga_info ((struct vga_info*)VGA_INFO)
void init_mm(void){
    init_page();
    init_table();
    alloc_table();  //Alloc PML table

    map_page(0,0);  //Init 4M
    map_page(PAGE_SIZE,PAGE_SIZE);

    asm(
	"mov rax,%0\n"
	"mov cr3,rax\n"
    ::"i"(PT_BASE):"rax");
}
