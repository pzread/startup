#include<bitop.h>

int test_bit(void *dst,unsigned long off){
    if(((unsigned long*)dst)[off / BITS_PER_LONG] & (1 << (off % BITS_PER_LONG))){
	return 1;
    }else{
	return 0;
    }
}
void set_bit(void *dst,unsigned long off){
    ((unsigned long*)dst)[off / BITS_PER_LONG] &= (1 << (off % BITS_PER_LONG));
}
void clear_bit(void *dst,unsigned long off){
    ((unsigned long*)dst)[off / BITS_PER_LONG] &= ~(1 << (off % BITS_PER_LONG));
}
unsigned long find_and_set_next_zero_bit(void *dst,
					unsigned long off,
					unsigned long size){
    unsigned long i;
    unsigned char j;

    unsigned long idx;
    unsigned long *bitmap;
    int rescan = 0;

    if(off >= size){
	return size;
    }

    bitmap = (unsigned long*)dst;
    i = off / BITS_PER_LONG;
    j = off % BITS_PER_LONG;
    idx = off;
    
    while(1){
	if(rescan == 1 && idx == off){
	    return size;
	}
	if(idx >= size){
	    if(rescan == 0){
		rescan = 1;

		i = 0;
		j = 0;
		idx = 0;
	    }else{
		return size;
	    }
	}
	
	if(j == BITS_PER_LONG){
	    i++;
	    j = 0;
	}
	
	if((bitmap[i] & (1 << j)) == 0){
	    set_bit(bitmap,idx);
	    return idx;
	}

	j++;
	idx++;
    }

    return size;
}
