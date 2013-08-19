#include<efi.h>
#include<efiprot.h>
//#include<efilib.h>

static void init_graphic(EFI_BOOT_SERVICES *boot_service){
    UINTN size;
    UINTN pages;
    EFI_HANDLE *handle;
    UINTN handle_idx;
    EFI_GUID graphic_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *graphic;
    UINT32 mode;
    UINT32 find_mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;

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

    find_mode = (UINT32)-1;
    for(handle_idx = size / sizeof(EFI_HANDLE) - 1;
	    handle_idx >= 0;handle_idx--){
	
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
	    
	    ////Print(L"%x %x %x %x\n",info->PixelInformation.RedMask,info->PixelInformation.GreenMask,info->PixelInformation.BlueMask,info->PixelInformation.ReservedMask);
	    if(info->HorizontalResolution >= 800 &&
		    info->VerticalResolution >= 600 &&
		    info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor){
		
		find_mode = mode;
		break;
	    }
	}

	if(find_mode != (UINT32)-1){
	    graphic->SetMode(graphic,find_mode);
	    //Print(L"%d x %d\n",graphic->Mode->Info->HorizontalResolution,graphic->Mode->Info->VerticalResolution);
	    //Print(L"%016lx %x %x\n",graphic->Mode->FrameBufferBase,graphic->Mode->FrameBufferSize,graphic->Mode->Info->PixelsPerScanLine);

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

    //InitializeLib(image_handle,system_table);

    boot_service = system_table->BootServices;
    init_graphic(boot_service);
    init_mem(boot_service);

    //Print(L"End\n");
    uefi_call_wrapper(system_table->ConOut->OutputString,2,system_table->ConOut,L"Hello UEFI\n\r");
    while(1);

    return EFI_SUCCESS;
}
