OUTPUT_FORMAT(binary)
SECTIONS{
    . = 0x0;
    .entry 0x7C00:{
	legacy_boot.o(.text)
    }
    .loader 0x7E00:{
	legacy_loader.o(*)
    }
}
