OUTPUT_FORMAT(binary)
ENTRY (main)
SECTIONS{
    . = 0x0;
    .kernel 0xFFFF800000010000:{
	kernel.o(*)
    }
}
