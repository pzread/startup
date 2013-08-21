#include<font.h>
#include<graphic.h>
#include<loader.h>

#define vga_info ((struct vga_info*)VGA_INFO)

void graphic_drawtext(unsigned int x,unsigned int y,char *text){
    int i;
    int j;
    int off;
    unsigned char *vmem;
    unsigned char *vchar;
    unsigned char data;

    vmem = (unsigned char*)(unsigned long)vga_info->vmem_base;

    while(*text != '\0'){
	vchar = (unsigned char*)(font_map + (unsigned long)(*text) * 16UL);
	
	for(i = 0;i < 16;i++){
	    off = x * 4 + (y + i) * vga_info->bytes_per_scanline;
	    data = vchar[i];
	    for(j = 0;j < 8;j++){
		if((data & 0x80) == 0x80){
		    vmem[off] = 0x0C;
		    vmem[off + 1] = 0x0C;
		    vmem[off + 2] = 0x0C;		
		}
		off += 4;
		data = data << 1;
	    }
	}

	x += 8;
	text++;
    }
}
