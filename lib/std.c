#include<loader.h>

#define vga_info ((struct vga_info*)VGA_INFO)

static char *log_prefix = "[startup kernel] ";
static int log_last_pos = 8;

static char digimap[16] = {'0','1','2','3','4','5','6','7','8','9',
    'A','B','C','D','E','F'}; 

void video_drawchar(int x,int y,char c){
    int i;
    int j;
    int off;
    unsigned char *vmem;
    unsigned char *vchar;
    unsigned char data;

    vmem = (unsigned char*)(unsigned long)vga_info->vmem_base;
    vchar = (unsigned char*)(VFONT_BASE + (unsigned long)c * 16UL);

    for(i = 0;i < 16;i++){
	off = (x + (y + i) * vga_info->x_res) * 3;
	data = vchar[i];
	for(j = 0;j < 8;j++){
	    if((data & 0x80) == 0){
		vmem[off] = 0x00;
		vmem[off + 1] = 0x00;
		vmem[off + 2] = 0x00;		
	    }else{
		vmem[off] = 0xE9;
		vmem[off + 1] = 0xE9;
		vmem[off + 2] = 0xE9;		
	    }
	    off += 3;
	    data = data << 1;
	}
    }
}
void video_drawtext(int x,int y,char *str){
    while(*str != '\0'){
	video_drawchar(x,y,*str);
	x += 8;
	str++;
    }
}
void std_sreverse(char *str,int len){
    int i;
    char tmp;

    for(i = len / 2 - 1;i >= 0;i--){
	tmp = str[i];
	str[i] = str[len - i - 1];
	str[len - i - 1] = tmp;
    }
}
void sprintf(char *str,char *fmt,unsigned long *args){
    int i;
    int j;
    int argi;
    int tmp;
    unsigned long u;

    argi = 0;
    while(*fmt != '\0'){
	if(*fmt == '%'){
	    fmt++;
	    if(*fmt == 'u'){
		u = args[argi];
		i = 0;
		do{
		    str[i] = digimap[u % 10];
		    u = u / 10;
		    i++;
		}while(u > 0);
		std_sreverse(str,i);

		str += i;
		fmt++;
		argi++;
	    }else if(*fmt == 'x'){
		u = args[argi];
		i = 0;
		do{
		    str[i] = digimap[u % 16];
		    u = u / 16;
		    i++;
		}while(u > 0);
		std_sreverse(str,i);

		str += i;
		fmt++;
		argi++;
	    }
	}else{
	    *str = *fmt;
	    str++;
	    fmt++;
	}
    }

    *str = '\0';
}

void log(char *msg){
    video_drawtext(8,log_last_pos,log_prefix);
    video_drawtext(8 + 8 * 17,log_last_pos,msg);
    log_last_pos += 16;
}
