OUTPUT_FORMAT(binary)
ENTRY (main)
SECTIONS{
    . = 0x0;
    .kernel 0xF000:{
	kernel.o(*)
    }
}
