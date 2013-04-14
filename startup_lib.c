#define VMEM_ADDR 0x500
#define VFONT_BASE 0x7F000

void video_drawchar(int x,int y,char c){
    int i;
    int j;
    int off;
    unsigned char *vmem;
    unsigned char *vchar;
    unsigned char data;

    vmem = (unsigned char*)*(unsigned long*)VMEM_ADDR;
    vchar = (unsigned char*)(VFONT_BASE + (unsigned long)c * 16UL);

    for(i = 0;i < 16;i++){
	off = (x + (y + i) * 1024) * 3;
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
void std_sprintf(char *str,char *fmt,unsigned long *args){
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
		    str[i] = (u % 10) + '0';
		    u = u / 10;
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
