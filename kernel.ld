OUTPUT_FORMAT(binary)
SECTIONS{
    . = 0x0;
    .kernel 0xFFFF800000010000:{
	kernel.o(.entry)
	*(COMMON)
    }
}
