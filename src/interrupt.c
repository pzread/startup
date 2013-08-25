#include<config.h>
#include<mm.h>
#include<acpi.h>
#include<loader.h>
#include<interrupt.h>

#define MAP_LAPIC KERNEL_MAP_APIC

#define LAPIC_SIZE  0x400
#define LAPIC_REG_TP	    0x80    //Task Priority
#define LAPIC_REG_EOI	    0xB0    //EOI
#define LAPIC_REG_LD	    0xD0    //Logical Destination
#define LAPIC_REG_DF	    0xE0    //Destination Format
#define LAPIC_REG_SIV	    0xF0    //Spurious Interrupt Vector
#define LAPIC_REG_LVT_CMCI  0x2F0   //LVT CMCI
#define LAPIC_REG_ICR	    0x300   //Interrupt Command Register
#define LAPIC_REG_LVT_TIMER 0x320   //LVT Timer
#define LAPIC_REG_LVT_TS    0x330   //LVT Thermal Sensor
#define LAPIC_REG_LVT_PMC   0x340   //LVT Performance Monitoring Counters
#define LAPIC_REG_LVT_LINT0 0x350   //LVT LINT0
#define LAPIC_REG_LVT_LINT1 0x360   //LVT LINT1
#define LAPIC_REG_LVT_ERR   0x370   //LVT Error
#define LAPIC_REG_IC	    0x380   //Initial Count Register
#define LAPIC_REG_CC	    0x390   //Current Count Register
#define LAPIC_REG_DC	    0x3E0   //Divide Configuration Register

#pragma pack(push)
#pragma pack(1)

struct idt_ptr{
    unsigned short limit;
    unsigned long base;
};

#pragma pack(pop)

extern void isr_null(void);

static void init_idt(void){
    int i;

    unsigned long *idt;
    unsigned long entry;
    unsigned long low;
    unsigned long high;
    __volatile__ struct idt_ptr idt_ptr;

    entry = (unsigned long)isr_null & LOW_MASK;
    low = 0x00008E0000000000UL | GDT_CODE |
	(entry & 0xFFFFUL) | ((entry & 0xFFFF0000UL) << 32UL);
    high = (entry & 0xFFFFFFFF00000000UL) >> 32UL;

    idt = (unsigned long*)IDT_BASE;
    for(i = 0;i < 512;i += 2){
	idt[i] = low;
	idt[i + 1] = high;
    }

    idt_ptr.limit = 16 * 256 - 1;
    idt_ptr.base = IDT_BASE & LOW_MASK;

    __asm__ __volatile__(
	"lidt [%0]\n"	    
    ::"g"(&idt_ptr):);
}
static void init_lapic(unsigned long base){
    *(unsigned int*)(base + LAPIC_REG_TP) = 0;
    *(unsigned int*)(base + LAPIC_REG_LD) = 0xFF000000;	    //Handle all interrupt
    *(unsigned int*)(base + LAPIC_REG_DF) = 0xFFFFFFFF;

    *(unsigned int*)(base + LAPIC_REG_LVT_TIMER) = 0x10000; //Disable
    *(unsigned int*)(base + LAPIC_REG_LVT_LINT0) = 0x10000;
    *(unsigned int*)(base + LAPIC_REG_LVT_LINT1) = 0x10000;
    *(unsigned int*)(base + LAPIC_REG_LVT_PMC) = 0x400;	    //NMI

    *(unsigned int*)(base + LAPIC_REG_SIV) = 0x100 | ISR_SPURIOUS;
    *(unsigned int*)(base + LAPIC_REG_DC) = 0xB;
    *(unsigned int*)(base + LAPIC_REG_LVT_TIMER) = 0x100000 | ISR_TIMER;
    *(unsigned int*)(base + LAPIC_REG_IC) = 0x100000;

    __asm__ __volatile__(
	"mov ecx,%0\n"
	"rdmsr\n"
	"or eax,%1\n"
	"wrmsr\n"
	"sti\n"
    ::"i"(MSR_APIC_BASE),"i"(MSRBIT_APIC_GE):);
}

void init_interrupt(void){
    map_pages(MAP_LAPIC,lapic_base,LAPIC_SIZE);
    
    init_idt();

    init_lapic(MAP_LAPIC + (lapic_base & (PAGE_SIZE - 1)));
}
