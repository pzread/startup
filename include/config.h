#define MAX_PROCESSOR 256

#define PAGE_SIZE 0x200000UL
#define PAGE_SHIFT 21UL

#define MSR_APIC_BASE 0x1B
#define MSRBIT_APIC_GE 0x800	//APIC Global Enable

#ifdef LOADER

#define HIGH_OFFSET 0
#define LOW_MASK -1

#else

#define HIGH_OFFSET 0xFFFF800000000000UL
#define LOW_MASK 0x7FFFFFFFFFFFUL

#endif

#ifndef NULL

#define NULL ((void*)0)

#endif
