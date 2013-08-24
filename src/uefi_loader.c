#define LOADER

#include<efi.h>
#include<efiprot.h>
#include<loader.h>

#define EFI_ACPI_TABLE_GUID {0xeb9d2d30,0x2d88,0x11d3, \
    {0x9a,0x16,0x0,0x90,0x27,0x3f,0xc1,0x4d}}
#define EFI_ACPI_20_TABLE_GUID {0x8868e871,0xe4f1,0x11d3, \
    {0xbc,0x22,0x0,0x80,0xc7,0x3c,0x88,0x81}}
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID {0x0964e5b22,0x6459,0x11d2, \
    {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}

struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
struct _EFI_FILE_PROTOCOL;
typedef EFI_STATUS (EFIAPI *EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME)(
	IN struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *This,
	OUT struct _EFI_FILE_PROTOCOL **Root);

#pragma pack(push)
#pragma pack(1)

typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL{
    UINT64 Revision;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
}EFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

typedef struct _EFI_FILE_PROTOCOL{
    UINT64 Revision;
    EFI_FILE_OPEN Open;
    EFI_FILE_CLOSE Close;
    EFI_FILE_DELETE Delete;
    EFI_FILE_READ Read;
    EFI_FILE_WRITE Write;
    EFI_FILE_GET_POSITION GetPosition;
    EFI_FILE_SET_POSITION SetPosition;
    EFI_FILE_GET_INFO GetInfo;
    EFI_FILE_SET_INFO SetInfo;
    EFI_FILE_FLUSH Flush;
}EFI_FILE_PROTOCOL;

struct gdt_ptr{
    unsigned short limit;
    unsigned long long base;
};

#pragma pack(pop)

static int memcmp(void *dst,void *src,unsigned long size){
    while(size > sizeof(unsigned long)){
	if(*(unsigned long*)(((unsigned char*)dst) +
		    size - sizeof(unsigned long)) !=
		*(unsigned long*)(((unsigned char*)src) +
		    size - sizeof(unsigned long))){

	    return 1;
	}
	size -= sizeof(unsigned long);
    }
    while(size > 0){
	if(*(((unsigned char*)dst) + size -1) !=
		*(((unsigned char*)src + size - 1))){

	    return 1;
	}
	size--;
    }

    return 0;
}
void memcpy(void *dst,void *src,unsigned long size){
    while(size > sizeof(unsigned long)){
	*(unsigned long*)(((unsigned char*)dst) +
		size - sizeof(unsigned long)) =
	    *(unsigned long*)(((unsigned char*)src) +
		size - sizeof(unsigned long));

	size -= sizeof(unsigned long);
    }
    while(size > 0){
	*(((unsigned char*)dst) + size - 1) =
	    *(((unsigned char*)src) + size - 1);

	size--;
    }
}

static unsigned int *tmp;
static void init_graphic(EFI_BOOT_SERVICES *boot_service){
    UINTN size;
    UINTN pages;
    EFI_HANDLE *handle;
    UINTN handle_num;
    UINTN handle_idx;
    EFI_GUID graphic_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphic;
    UINT32 mode;
    UINT32 find_mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;

    __volatile__ struct vga_info *vga_info;

    size = 0;
    uefi_call_wrapper(boot_service->LocateHandle,
	    5,
	    ByProtocol,
	    &graphic_guid,
	    NULL,
	    &size,
	    NULL);

    pages = size / 4096 + 1;
    uefi_call_wrapper(boot_service->AllocatePages,
	    4,
	    AllocateAnyPages,
	    EfiLoaderData,
	    pages,
	    (EFI_PHYSICAL_ADDRESS*)&handle);

    uefi_call_wrapper(boot_service->LocateHandle,
	    5,
	    ByProtocol,
	    &graphic_guid,
	    NULL,
	    &size,
	    handle);

    handle_num = size / sizeof(EFI_HANDLE);
    find_mode = (UINT32)-1;
    for(handle_idx = 0;handle_idx < handle_num;handle_idx++){
	if(uefi_call_wrapper(boot_service->HandleProtocol,
		    3,
		    handle[handle_idx],
		    &graphic_guid,
		    (VOID**)&graphic) != EFI_SUCCESS){

	    continue;
	}

	for(mode = 0;mode < graphic->Mode->MaxMode;mode++){
	    if(uefi_call_wrapper(graphic->QueryMode,
			4,
			graphic,
			mode,
			&size,
			&info) != EFI_SUCCESS){

		continue;
	    }

	    if(info->HorizontalResolution >= 800 &&
		    info->VerticalResolution >= 600 &&
		    info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor){

		find_mode = mode;
		break;
	    }
	}

	if(find_mode != (UINT32)-1){
	    graphic->SetMode(graphic,find_mode);

	    vga_info = (struct vga_info*)VGA_INFO;
	    vga_info->vmem_base = graphic->Mode->FrameBufferBase;
	    vga_info->x_res = graphic->Mode->Info->HorizontalResolution;
	    vga_info->y_res = graphic->Mode->Info->VerticalResolution;
	    vga_info->bits = 32;
	    vga_info->bytes_per_scanline = graphic->Mode->Info->PixelsPerScanLine * 4;

	    tmp = (unsigned int*)graphic->Mode->FrameBufferBase;

	    break;
	}
    }

    uefi_call_wrapper(boot_service->FreePages,
	    2,
	    (EFI_PHYSICAL_ADDRESS)handle,
	    pages);

    if(find_mode == (UINT32)-1){
	while(1);
    }
}
static void init_rsdp(EFI_SYSTEM_TABLE *system_table){
    int i;

    EFI_CONFIGURATION_TABLE *conf_table;
    EFI_GUID acpi1_guid = EFI_ACPI_TABLE_GUID;
    EFI_GUID acpi2_guid = EFI_ACPI_20_TABLE_GUID;
    struct rsdp *rsdp;

    rsdp = NULL;
    conf_table = system_table->ConfigurationTable;
    for(i = 0;i < system_table->NumberOfTableEntries;i++){
	if(!memcmp(&conf_table[i].VendorGuid,&acpi2_guid,sizeof(EFI_GUID))){
	    rsdp = conf_table[i].VendorTable;
	    break;
	}    
	if(!memcmp(&conf_table[i].VendorGuid,&acpi1_guid,sizeof(EFI_GUID))){
	    rsdp = conf_table[i].VendorTable;
	}
    }

    if(rsdp == NULL){
	while(1);
    }
    
    memcpy((void*)RSDP_INFO,rsdp,sizeof(struct rsdp));
}
static void init_kernel(EFI_BOOT_SERVICES *boot_service){
    UINTN size;
    UINTN pages;
    EFI_HANDLE *handle;
    UINTN handle_num;
    UINTN handle_idx;
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    EFI_FILE_PROTOCOL *root;
    EFI_FILE_PROTOCOL *file;

    size = 0;
    uefi_call_wrapper(boot_service->LocateHandle,
	    5,
	    ByProtocol,
	    &fs_guid,
	    NULL,
	    &size,
	    NULL);

    pages = size / 4096 + 1;
    uefi_call_wrapper(boot_service->AllocatePages,
	    4,
	    AllocateAnyPages,
	    EfiLoaderData,
	    pages,
	    (EFI_PHYSICAL_ADDRESS*)&handle);

    uefi_call_wrapper(boot_service->LocateHandle,
	    5,
	    ByProtocol,
	    &fs_guid,
	    NULL,
	    &size,
	    handle);

    handle_num = size / sizeof(EFI_HANDLE);
    for(handle_idx = 0;handle_idx < handle_num;handle_idx++){
	if(uefi_call_wrapper(boot_service->HandleProtocol,
		    3,
		    handle[handle_idx],
		    &fs_guid,
		    (VOID**)&fs) != EFI_SUCCESS){

	    continue;
	}

	if(uefi_call_wrapper(fs->OpenVolume,
		2,
		fs,
		&root) != EFI_SUCCESS){
	    
	    continue;
	}

	if(uefi_call_wrapper(root->Open,
		5,
		root,
		&file,
		L"efi\\kernel.img",
		EFI_FILE_MODE_READ,
		0) != EFI_SUCCESS){
	    
	    uefi_call_wrapper(root->Close,
		1,
		root);

	    continue;
	}

	size = 32768;
	uefi_call_wrapper(file->Read,
		3,
		file,
		&size,
		(VOID*)0xF000);

	uefi_call_wrapper(file->Close,
		1,
		file);
	
	uefi_call_wrapper(root->Close,
		1,
		root);

	break;
    }

    uefi_call_wrapper(boot_service->FreePages,
	    2,
	    (EFI_PHYSICAL_ADDRESS)handle,
	    pages);
}
static void init_resource(EFI_BOOT_SERVICES *boot_service){
    UINTN size;
    UINTN pages;
    EFI_HANDLE *handle;
    UINTN handle_num;
    UINTN handle_idx;
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    EFI_FILE_PROTOCOL *root;
    EFI_FILE_PROTOCOL *file;

    size = 0;
    uefi_call_wrapper(boot_service->LocateHandle,
	    5,
	    ByProtocol,
	    &fs_guid,
	    NULL,
	    &size,
	    NULL);

    pages = size / 4096 + 1;
    uefi_call_wrapper(boot_service->AllocatePages,
	    4,
	    AllocateAnyPages,
	    EfiLoaderData,
	    pages,
	    (EFI_PHYSICAL_ADDRESS*)&handle);

    uefi_call_wrapper(boot_service->LocateHandle,
	    5,
	    ByProtocol,
	    &fs_guid,
	    NULL,
	    &size,
	    handle);

    handle_num = size / sizeof(EFI_HANDLE);
    for(handle_idx = 0;handle_idx < handle_num;handle_idx++){
	if(uefi_call_wrapper(boot_service->HandleProtocol,
		    3,
		    handle[handle_idx],
		    &fs_guid,
		    (VOID**)&fs) != EFI_SUCCESS){

	    continue;
	}

	if(uefi_call_wrapper(fs->OpenVolume,
		2,
		fs,
		&root) != EFI_SUCCESS){
	    
	    continue;
	}

	if(uefi_call_wrapper(root->Open,
		5,
		root,
		&file,
		L"efi\\tmp.data",
		EFI_FILE_MODE_READ,
		0) != EFI_SUCCESS){
	    
	    uefi_call_wrapper(root->Close,
		1,
		root);

	    continue;
	}

	size = 1024 * 768 * 4;
	uefi_call_wrapper(file->Read,
		3,
		file,
		&size,
		(VOID*)0x400000);

	uefi_call_wrapper(file->Close,
		1,
		file);
	
	uefi_call_wrapper(root->Close,
		1,
		root);

	break;
    }

    uefi_call_wrapper(boot_service->FreePages,
	    2,
	    (EFI_PHYSICAL_ADDRESS)handle,
	    pages);
}
static void init_memory_and_exit(
	EFI_BOOT_SERVICES *boot_service,
	EFI_HANDLE image_handle){

    UINTN size;
    UINTN desc_size;
    UINTN pages;
    EFI_MEMORY_DESCRIPTOR *desc;
    UINTN map_key;
    
    __volatile__ struct mem_info *mem_info;

    size = 0;
    uefi_call_wrapper(boot_service->GetMemoryMap,
	    5,
	    &size,
	    NULL,
	    NULL,
	    &desc_size,
	    NULL);

    size += desc_size * 16;
    pages = size / 4096 + 1;
    uefi_call_wrapper(boot_service->AllocatePages,
	    4,
	    AllocateAnyPages,
	    EfiLoaderData,
	    pages,
	    (EFI_PHYSICAL_ADDRESS*)&desc);

    mem_info = (struct mem_info*)MEM_INFO;
    mem_info->region_count = 0;

    while(1){
	size = pages * 4096;
	uefi_call_wrapper(boot_service->GetMemoryMap,
		5,
		&size,
		desc,
		&map_key,
		NULL,
		NULL);

	while(size > 0){
	    mem_info->region[mem_info->region_count].base =
		desc->PhysicalStart;
	    mem_info->region[mem_info->region_count].size =
		desc->NumberOfPages * 4096UL;

	    if(desc->Type == EfiConventionalMemory){
		mem_info->region[mem_info->region_count].type =
		    MEM_TYPE_USABLE;
	    }else{
		mem_info->region[mem_info->region_count].type =
		    MEM_TYPE_RESERVED;
	    }
	    
	    mem_info->region_count += 1;
	    desc = (EFI_MEMORY_DESCRIPTOR*)(((char*)desc) + desc_size);
	    size -= desc_size;
	}

	if(uefi_call_wrapper(boot_service->ExitBootServices,
		    2,
		    image_handle,
		    map_key) == EFI_SUCCESS){

	    break;
	}
    }
}
void prepare_kernel(EFI_RUNTIME_SERVICES *runtime_service){
    int i;
    int j;

    __volatile__ unsigned int *intermap;
    __volatile__ unsigned int *iomap;
    __volatile__ unsigned int *tss;
    unsigned short iomap_off;

    __volatile__ unsigned long *gdt;
    __volatile__ struct gdt_ptr gdt_ptr;

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

    gdt = (unsigned long*)GDT_BASE;
    gdt[0] = 0;
    gdt[1] = 0x20980000000000;
    gdt[2] = 0x900000000000;

    for(i = 0,j = 3;i < MAX_PROCESSOR;i++,j += 2){
	gdt[j] = 0x89000000D5FF | ((TSS_BASE + i * 104) << 16);
	gdt[j + 1] = 0;
    }

    gdt_ptr.limit = 24 + 16 * MAX_PROCESSOR - 1;
    gdt_ptr.base = GDT_BASE;

    __asm__ __volatile__(
	"mov rax,cr4\n"	//Enable PAE
	"or rax,0x20\n"
	"mov cr4,rax\n"	
	"mov rcx,0xC0000080\n"	//Enable LME
	"rdmsr\n"
	"or rax,0x100\n"
	"wrmsr\n"
	"cli\n"
	"mov rax,cr0\n"	//Enable long mode
	"or eax,0x80000001\n"
	"mov cr0,rax\n"
	"lgdt [%0]\n"	//Load GDT
	"mov rax,%1\n"	//Load TSS
	"ltr ax\n"
	"mov ax,%2\n"	//Init fs,gs
	"mov fs,ax\n"
	"mov gs,ax\n"
    ::"g"(&gdt_ptr),"i"(GDT_TSSD_BASE),"i"(GDT_DATA):"rax","rcx","rdx");
}

EFI_STATUS efi_main(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *system_table){
    int i,j,k;

    EFI_BOOT_SERVICES *boot_service;

    boot_service = system_table->BootServices;

    init_graphic(boot_service);
    init_rsdp(system_table);
    init_kernel(boot_service);

    init_resource(boot_service);
    k = 0;
    for(i = 0;i < 768;i++){
	for(j = 0;j < 512;j++){
	    ((unsigned long*)tmp)[i * 512 + j] = ((unsigned long*)0x400000)[k]; 
	    k++;
	}
    }

    init_memory_and_exit(boot_service,image_handle);
    prepare_kernel(system_table->RuntimeServices);

    __asm__ __volatile__(
	"mov rax,0xF000\n"
	"jmp rax\n"
    :::);

    return EFI_SUCCESS;
}
