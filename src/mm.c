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

struct mblock{
    unsigned long tag;
    struct list_head list;
};

#define MBLOCK_TAG_USED 0x1
#define MBLOCK_TAG_SIZE ~3UL

#define MBLOCK_SIZE (sizeof(struct mblock) + sizeof(unsigned long))

#define MBLOCK_TAILTAG(block) \
    (*(unsigned long*)(((char*)(block)) + \
    ((block)->tag & MBLOCK_TAG_SIZE) - sizeof(unsigned long)))

#define MBLOCK_PREVTAG(block) \
    (*(unsigned long*)(((char*)(block)) - sizeof(unsigned long)))

#define MBLOCK_NEXTTAG(block) \
    (*(unsigned long*)(((char*)(block)) + ((block)->tag & MBLOCK_TAG_SIZE)))

static unsigned long page_idx_next;
static unsigned long table_idx_next;

static unsigned long kheap_start;
static unsigned long kheap_end;
static struct list_head kmem_free_list[16];
static unsigned int kmem_free_size[15] = {0x40,0x80,0x100,0x200,0x400,0x800,0x1000,0x2000,0x4000,0x8000,0x10000,0x20000,0x40000,0x80000,0x100000};

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
    __volatile__ unsigned long *pdpte;
    __volatile__ unsigned long *pde;

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

	__asm__ __volatile__(	//Invalidate TLB, reload PDPTE registers
	    "mov rax,cr3\n"
	    "mov cr3,rax\n"
	:::"rax");
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

static void* alloc_kheap(unsigned long size){
    void *ret;
    unsigned long page;
    unsigned long end;

    if(size == 0){
	return ret;
    }
    size = ((size - 1UL) & (~15UL)) + 16UL;

    end = kheap_start + size + sizeof(unsigned long);
    while(kheap_end < end){
	page = alloc_page();
	map_page(kheap_end,(unsigned long)page);
	kheap_end += PAGE_SIZE;
    }

    ret = (void*)kheap_start;
    kheap_start += size;
    *(unsigned long*)kheap_start = MBLOCK_TAG_USED;
    
    return ret;
}
static void init_kmem(void){
    int i;

    kheap_start = 0xFFFF800000400000UL;
    kheap_end = kheap_start;
    alloc_kheap(16);
    MBLOCK_PREVTAG(kheap_start) = MBLOCK_TAG_USED;

    for(i = 0;i < 16;i++){
	INIT_LIST_HEAD(kmem_free_list[i]);
    }
}
static get_free_head(unsigned long size){
    int i;

    for(i = 0;i < 15;i++){
	if(size <= kmem_free_size[i]){
	    break;
	}
    }

    return i;
}
static void merge_free_mblock(struct mblock **block){
    struct mblock *close_block;
    struct mblock *new_block;

    list_del(&(*block)->list);

    close_block = *block;
    while(!(MBLOCK_PREVTAG(close_block) & MBLOCK_TAG_USED)){
	close_block = (struct mblock*)((char*)close_block -
		(MBLOCK_PREVTAG(close_block) & MBLOCK_TAG_SIZE));
	list_del(&close_block->list);
    }
    new_block = close_block;

    close_block = *block;
    while(!(MBLOCK_NEXTTAG(close_block) & MBLOCK_TAG_USED)){
	close_block = (struct mblock*)((char*)close_block +
		(close_block->tag & MBLOCK_TAG_SIZE));
	list_del(&close_block->list);
    }

    new_block->tag = (unsigned long)close_block - (unsigned long)new_block +
	(close_block->tag & MBLOCK_TAG_SIZE);
    MBLOCK_TAILTAG(new_block) = new_block->tag;

    *block = new_block;
}

void* kmalloc(unsigned long size){
    int idx;

    unsigned long block_size;
    unsigned long old_size;
    struct list_head *free_head;
    struct mblock *block;
    struct mblock *old_block;
    int find_flag;

    if(size == 0){
	return NULL;
    }
    block_size = size + MBLOCK_SIZE;
    block_size = ((block_size - 1UL) & (~63UL)) + 64UL; 
    
    find_flag = 0;
    for(idx = 0;idx < 15;idx++){
	if(size <= kmem_free_size[idx] && !list_empty(&kmem_free_list[idx])){
	    find_flag = 1;
	    break;
	}
    }

    free_head = &kmem_free_list[idx];
    if(find_flag == 0 && !list_empty(free_head)){
	list_for_each_entry(block,free_head,list){
	    if((block->tag & MBLOCK_TAG_SIZE) >= block_size){
		find_flag = 1;
		break;
	    }
	}
    }

    if(find_flag == 0){
	block = (struct mblock*)alloc_kheap(block_size);
	block->tag = block_size | MBLOCK_TAG_USED;
	MBLOCK_TAILTAG(block) = block->tag;
	INIT_LIST_HEAD(block->list);
    }else{
	block = container_of(free_head->next,struct mblock,list);
	list_del(&block->list);

	old_size = block->tag & MBLOCK_TAG_SIZE;
	old_size -= block_size;
	if(old_size > 0){
	    old_block = (struct mblock*)(((char*)block) + block_size);

	    old_block->tag = old_size;
	    MBLOCK_TAILTAG(old_block) = old_block->tag;
	    
	    list_add(&old_block->list,&kmem_free_list[get_free_head(old_size)]);
	}
	
	block->tag = block_size;
	MBLOCK_TAILTAG(block) = block->tag;
    }

    return (void*)(((char*)block) + sizeof(struct mblock));
}
void kfree(void *ptr){
    struct mblock *block;
    struct list_head *free_head;

    block = (struct mblock*)(((char*)ptr) - sizeof(struct mblock));
    block->tag &= ~MBLOCK_TAG_USED;
    MBLOCK_TAILTAG(block) = block->tag;

    merge_free_mblock(&block);

    free_head = &kmem_free_list[get_free_head(block->tag & MBLOCK_TAG_SIZE)];
    list_add(&block->list,free_head);
}

void init_mm(void){
    init_page();
    init_table();
    alloc_table();  //Alloc PML table

    map_page(0xFFFF800000000000,0);  //Init 4M memory
    map_page(0xFFFF800000000000 + PAGE_SIZE,PAGE_SIZE);

    __asm__ __volatile__(
	"mov rax,%0\n"
	"mov cr3,rax\n"
    ::"i"(PT_BASE & 0x7FFFFFFFF000):"rax","memory");

    init_kmem();
}
