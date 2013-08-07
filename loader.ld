OUTPUT_FORMAT(binary)
ENTRY (entry)
SECTIONS{
    . = 0x0;
    .entry 0x7C00:{
	entry.o(.text)
    }
    .loader 0x7E00:{
	loader.o(*)
    }
}
