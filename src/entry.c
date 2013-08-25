#include<config.h>

extern void main(void);

void entry(void){
    int i;

    __volatile__ unsigned long *pml;
    __volatile__ unsigned long *pdpte;
    __volatile__ unsigned long *pde;

    pml = (unsigned long*)0x7D000;  //Init temporarily page table
    pdpte = (unsigned long*)0x7E000;
    pde = (unsigned long*)0x7F000;

    for(i = 2;i < 512;i++){
	pde[i] = 0;
    }
    pde[0] = 0x0 | 0x83;
    pde[1] = 0x200000 | 0x83;

    for(i = 1;i < 512;i++){
	pdpte[i] = 0;
    }
    pdpte[0] = 0x7F000 | 0x3;

    for(i = 1;i < 512;i++){
	pml[i] = 0;
    }
    pml[0] = 0x7E000 | 0x3;
    pml[256] = 0x7E000 | 0x3;
    
    __asm__ __volatile__(
	"mov rax,%0\n"
	"mov cr3,rax\n"
    ::"g"(pml):"rax");

    __asm__ __volatile__(
	"mov rsp,%0\n"
    ::"i"(HIGH_OFFSET | 0x7DC0):);

    main();
}
