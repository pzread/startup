#include<config.h>
#include<mm.h>
#include<std.h>
#include<loader.h>
#include<acpi.h>

#define MAP_RSDT KERNEL_MAP_ACPI
#define MAP_XSDT KERNEL_MAP_ACPI
#define MAP_DESC KERNEL_MAP_ACPI + 0x400000UL

#define APIC_TYPE_LAPIC 0
#define APIC_TYPE_IOAPIC 1

#pragma pack(push)
#pragma pack(1)

struct description_header{
    unsigned int signature;
    unsigned int length;
    unsigned char revision;
    unsigned char checksum;
    unsigned char oemid[6];
    unsigned char oem_tableid[8];
    unsigned int oem_revision;
    unsigned int creator_id;
    unsigned int creator_revision;
};
struct rsdt{
    unsigned int signature;
    unsigned int length;
    unsigned char revision;
    unsigned char checksum;
    unsigned char oemid[6];
    unsigned char oem_tableid[8];
    unsigned int oem_revision;
    unsigned int creator_id;
    unsigned int creator_revision;
};
struct xsdt{
    unsigned int signature;
    unsigned int length;
    unsigned char revision;
    unsigned char checksum;
    unsigned char oemid[6];
    unsigned char oem_tableid[8];
    unsigned int oem_revision;
    unsigned int creator_id;
    unsigned int creator_revision;
};

struct madt{
    struct description_header desc_header;
    unsigned int lic_base;	//Local Interrupt Controller Address
    unsigned int flags;
};
struct madt_lapic{
    unsigned char type;
    unsigned char length;
    unsigned char apic_processorid;
    unsigned char apic_id;
    unsigned int flags;
};
struct madt_ioapic{
    unsigned char type;
    unsigned char length;
    unsigned char ioapic_id;
    unsigned char reserved;
    unsigned int ioapic_base;
    unsigned int gsi_base;	//Global System Interrput Base
};

#pragma pack(pop)

unsigned int processor_num;
struct processor processor[MAX_PROCESSOR];
unsigned long lapic_base;
unsigned long ioapic_base;

unsigned long args[2];
char str[64];

static void init_apic(struct madt* madt){
    unsigned char *base;
    unsigned long end;

    struct madt_lapic *lapic;
    struct madt_ioapic *ioapic;

    processor_num = 0;
    lapic_base = madt->lic_base;

    base = (unsigned char*)madt + sizeof(*madt);
    end = (unsigned long)madt + madt->desc_header.length;
    while((unsigned long)base < end){
	switch(base[0]){
	    case APIC_TYPE_LAPIC:
		lapic = (struct madt_lapic*)base;

		processor_num += 1;
		processor[lapic->apic_id].apic_id = lapic->apic_id;

		break;
	    case APIC_TYPE_IOAPIC:
		ioapic = (struct madt_ioapic*)base;
		ioapic_base = ioapic->ioapic_base;

		break;
	}

	base += base[1];
    }
}

void init_acpi(void){
    struct rsdp *rsdp;
    struct rsdt *rsdt;
    struct xsdt *xsdt;
    unsigned long idx;
    unsigned long count;
    unsigned long *entry;
    struct description_header *desc_header;

    rsdp = (struct rsdp*)RSDP_INFO;
    if(rsdp->revision == 0){	//ACPI 1.0
	
    }else{  //ACPI 2.0
	map_pages(MAP_XSDT,rsdp->xsdt_base,sizeof(*xsdt));
	xsdt = (struct xsdt*)(MAP_XSDT | (rsdp->xsdt_base & (PAGE_SIZE - 1)));
	map_pages(MAP_XSDT,rsdp->xsdt_base,xsdt->length);

	entry = (unsigned long*)((unsigned long)xsdt + sizeof(*xsdt));
	count = (xsdt->length - sizeof(*xsdt)) / sizeof(unsigned long);
	for(idx = 0;idx < count;idx++){
	    map_pages(MAP_DESC,entry[idx],sizeof(*desc_header));
	    desc_header = (struct description_header*)(MAP_DESC +
		    (entry[idx] & (PAGE_SIZE - 1)));
	    map_pages(MAP_DESC,entry[idx],desc_header->length);

	    if(desc_header->signature == 0x43495041){
		init_apic((struct madt*)desc_header);
	    }
	}
    }
}
