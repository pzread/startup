#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int main(int argc,char *argv[]){
    int size;

    sscanf(argv[2],"%d",&size);
    truncate(argv[1],size);
    
    return 0;
}
