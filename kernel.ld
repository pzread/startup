OUTPUT_FORMAT(binary)
SECTIONS{
    . = 0x0;
    .entry 0xF000: AT (0x0){
	entry.o(*)
    }
    .kernel 0xFFFF800000010000: AT (0x1000){
	*.o(*)
    }
}
