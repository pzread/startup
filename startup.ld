OUTPUT_FORMAT(binary)
ENTRY (loader)
SECTIONS{
    . = 0x0;
    .loader 0x7C00:{
	startup_loader.o(.text)
    }
    .kernel 0x7E00:{
	startup_kernel.o(.text)
	startup_kernel.o(.data)
	startup_lib.o(*)
    }
    .ap 0x9000:{
	startup_kernel.o(.aptext)
    }
}
