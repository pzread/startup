struct processor{
    unsigned int apic_id;
};

extern unsigned int processor_num;
extern struct processor processor[MAX_PROCESSOR];
extern unsigned long lapic_base;
extern unsigned long ioapic_base;
