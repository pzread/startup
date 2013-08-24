#include<loader.h>

static char *log_prefix = "[startup kernel] ";
static int log_last_pos = 8;

static char digimap[16] = {'0','1','2','3','4','5','6','7','8','9',
    'A','B','C','D','E','F'}; 

void memset(void *dst,char value,unsigned long size){
    int i;
    unsigned long long_value;

    long_value = 0;
    for(i = 0;i < sizeof(unsigned long);i++){
	long_value = long_value << sizeof(unsigned char);
	long_value += (unsigned long)value;
    }
    while(size > sizeof(unsigned long)){
	*(unsigned long*)dst = long_value;	
	dst = ((unsigned long*)dst) + 1;
	size -= sizeof(unsigned long);
    }
    while(size > 0){
	*(unsigned char*)dst = value;
	dst = ((unsigned char*)dst) + 1;
	size -= sizeof(unsigned char);
    }
}
int memcmp(void *dst,void *src,unsigned long size){
    while(size > sizeof(unsigned long)){
	if(*(unsigned long*)(((unsigned char*)dst) +
		    size - sizeof(unsigned long)) !=
		*(unsigned long*)(((unsigned char*)src) +
		    size - sizeof(unsigned long))){

	    return 1;
	}
	size -= sizeof(unsigned long);
    }
    while(size > 0){
	if(*(((unsigned char*)dst) + size -1) !=
		*(((unsigned char*)src + size - 1))){

	    return 1;
	}
	size--;
    }

    return 0;
}
void memcpy(void *dst,void *src,unsigned long size){
    while(size > sizeof(unsigned long)){
	*(unsigned long*)(((unsigned char*)dst) +
		size - sizeof(unsigned long)) =
	    *(unsigned long*)(((unsigned char*)src) +
		size - sizeof(unsigned long));

	size -= sizeof(unsigned long);
    }
    while(size > 0){
	*(((unsigned char*)dst) + size - 1) =
	    *(((unsigned char*)src) + size - 1);

	size--;
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
    graphic_drawtext(8,log_last_pos,log_prefix);
    graphic_drawtext(8 + 8 * 17,log_last_pos,msg);
    log_last_pos += 16;
}
