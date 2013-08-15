#include<std.h>
#include<list.h>
#include<bitop.h>
#include<mm.h>
#include<err.h>
#include<loader.h>

#define PT_PAGEMAP 0xFFFF8000001E8000UL
#define PT_PAGEMAP_SIZE 32768UL
#define PT_MAX_PAGE (PT_PAGEMAP_SIZE * 8UL)

#define PT_TABLEMAP 0xFFFF8000001F0000UL
#define PT_TABLEMAP_SIZE 65536UL
#define	PT_BASE 0xFFFF800000200000UL
#define PT_MAX_TABLE 512UL
#define PT_TABLE_SIZE 4096UL

#define PT_FLAG_P 0x1UL
#define PT_FLAG_RW 0x2UL
#define PT_FLAG_US 0x4UL
#define PT_FLAG_PS 0x80
#define PT_FLAG_G 0x100UL
#define PT_FLAG_XD 0x8000000000000000UL

#define PT_PML_TABLE ((unsigned long*)PT_BASE)
#define PT_SET_ENTRY(e,x,f) (e) = ((unsigned long)(x) & 0x7FFFFFFFF000UL) | (unsigned long)((f) | PT_FLAG_P)

/*
Memory Block

#Tag
#List_head
#Payload
#Tag

Tag: Size + low 2bit for tag

Memort Free list
64B ~ 65536B,> 65536B
*/

struct mblock{
    unsigned long tag;
    struct list_head list;
};

#define MBLOCK_SIZE (sizeof(struct mblock) + sizeof(unsigned long))
#define MBLOCK_TAILTAG(block,size) \
    (*(unsigned long*)(((char*)(block)) + (size) - sizeof(unsigned long)))
#define MBLOCK_TAG_USED 0x1
#define MBLOCK_TAG_CLOSE_USED 0x2
#define MBLOCK_TAG_SIZE ~3UL

static unsigned long page_idx_next;
static unsigned long table_idx_next;

static void *kheap_start;
static void *kheap_end;
static struct list_head kmem_free_list[16];
static unsigned int kmem_free_size[15] = {64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576};

static void init_page(void){
    int i;

    struct mem_info *mem_info;
    unsigned long base;
    unsigned long size;
    unsigned long off;
    unsigned long idx;
    
    memset((void*)PT_PAGEMAP,-1,PT_PAGEMAP_SIZE);

    mem_info = (struct mem_info*)MEM_INFO;
    page_idx_next = 0;
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
		if(idx >= 2){	//Low 4M for kernel init
		    clear_bit((void*)PT_PAGEMAP,idx);
		    if(page_idx_next == 0){
			page_idx_next = idx;
		    }
		}

		idx += 1;
		size -= PAGE_SIZE;
	    }
	}
    }
}
static unsigned long alloc_page(void){
    unsigned long idx;

    idx = find_and_set_next_zero_bit((void*)PT_PAGEMAP,page_idx_next,
	    PT_MAX_PAGE);

    if(idx < PT_MAX_PAGE){
	return idx * PAGE_SIZE;
    }

    return (unsigned long)-1;
}

static void init_table(void){
    memset((void*)PT_TABLEMAP,0,PT_TABLEMAP_SIZE);
    table_idx_next = 0;
}
static unsigned long* alloc_table(void){
    unsigned long idx;
    unsigned long *table = NULL;

    idx = find_and_set_next_zero_bit((void*)PT_TABLEMAP,table_idx_next,
	    PT_MAX_TABLE);

    if(idx < PT_MAX_TABLE){
	table = (unsigned long*)(((char*)PT_BASE) + (idx * PT_TABLE_SIZE));
	table_idx_next = idx + 1;
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
	//Direct translate phys to virtual
	pdpte = (unsigned long*)(entry & 0xFFFFFFFFF000 | 0xFFFF800000000000);
    }

    entry = pdpte[pdpte_idx];
    if(!(entry & PT_FLAG_P)){
	if((pde = alloc_table()) == NULL){
	    return -ENOMEM;
	}
	PT_SET_ENTRY(entry,pde,PT_FLAG_RW);
	pdpte[pdpte_idx] = entry;
    }else{
	pde = (unsigned long*)(entry & 0xFFFFFFFFF000 | 0xFFFF800000000000);
    }
    
    PT_SET_ENTRY(pde[pde_idx],src & 0xFFFFFFE00000,PT_FLAG_PS | PT_FLAG_RW);

    return 0;
}

static void init_kmem(void){
    int i;

    kheap_start = (void*)0xFFFF800000400000;
    kheap_end = kheap_start;

    for(i = 0;i < 16;i++){
	INIT_LIST_HEAD(kmem_free_list[i]);
    }
}
static void* expend_kheap(unsigned long size,unsigned long *ret_len){
    void *ret = kheap_end;
    unsigned long page;

    if(size == 0){
	return ret;
    }
    size = ((size - 1) & (~(PAGE_SIZE - 1))) + PAGE_SIZE;
    if(ret_len != NULL){
	*ret_len = size;
    }

    while(size > 0){
	page = alloc_page();
	map_page((unsigned long)kheap_end,(unsigned long)page);

	kheap_end += PAGE_SIZE;
	size -= PAGE_SIZE;
    }

    return ret;
}
void* kmalloc(unsigned long size){
    int i;
    int j;
    unsigned long block_size;
    struct list_head *free_head;
    struct mblock *block;
    int find_flag;

    block_size = size + MBLOCK_SIZE;
    for(i = 14;i >= 0;i--){
	if(block_size > kmem_free_size[i]){
	    break;
	}
    }
    i += 1;

    free_head = &kmem_free_list[i];
    if(i < 15){
	if(list_empty(free_head)){
	    block_size = kmem_free_size[i];
	    block = (struct mblock*)expend_kheap(PAGE_SIZE,NULL);

	    for(j = (PAGE_SIZE / block_size) - 1;j >= 0;j--){
		block->tag = block_size;
		MBLOCK_TAILTAG(block,block_size) = block_size;
		list_add(&block->list,free_head);

		block = (struct mblock*)(((char*)block) + block_size);
	    }
	}

	block = container_of(free_head->next,struct mblock,list);
	block->tag |= MBLOCK_TAG_USED;
	MBLOCK_TAILTAG(block,block_size) = block->tag;
	list_del(free_head->next);
    }else{
	block = NULL;
	find_flag = 0;
	list_for_each_entry(block,free_head,list){
	    if((block->tag & MBLOCK_TAG_SIZE) >= block_size){
		find_flag = 1;
		break;
	    }
	}
	if(!find_flag){
	    block = (struct mblock*)expend_kheap(block_size,&block_size);
	    block->tag = block_size | MBLOCK_TAG_USED;
	    MBLOCK_TAILTAG(block,block_size) = block->tag;
	}
    }

    return (void*)(((char*)block) + sizeof(struct mblock));
}

void init_mm(void){
    init_page();
    init_table();
    alloc_table();  //Alloc PML table

    map_page(0xFFFF800000000000,0);  //Init 4M
    map_page(0xFFFF800000000000 + PAGE_SIZE,PAGE_SIZE);

    __asm__ __volatile__(
	"mov rax,%0\n"
	"mov cr3,rax\n"
    ::"i"(PT_BASE & 0x7FFFFFFFF000UL):"rax","memory");

    init_kmem();
}
