#include<efi.h>
#include<efilib.h>

static void init_graphic(){
    
}
static void init_mem(EFI_BOOT_SERVICES *boot_service){
    UINTN size;
    UINTN desc_size;
    UINTN pages;
    EFI_MEMORY_DESCRIPTOR *desc;
    
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

    uefi_call_wrapper(boot_service->GetMemoryMap,
	    5,
	    &size,
	    desc,
	    NULL,
	    NULL,
	    NULL);

    while(size > 0){
	if(desc->Type == EfiConventionalMemory){
	
	}

	desc = (EFI_MEMORY_DESCRIPTOR*)(((char*)desc) + desc_size);
	size -= desc_size;
    }

    uefi_call_wrapper(boot_service->FreePages,
	    2,
	    (EFI_PHYSICAL_ADDRESS)desc,
	    pages);
}

EFI_STATUS efi_main(EFI_HANDLE image_handle,EFI_SYSTEM_TABLE *system_table){
    EFI_BOOT_SERVICES *boot_service;

    uefi_call_wrapper(system_table->ConOut->OutputString,2,system_table->ConOut,L"Hello UEFI\n\r");

    boot_service = system_table->BootServices;
    init_mem(boot_service);

    while(1);

    return EFI_SUCCESS;
}
