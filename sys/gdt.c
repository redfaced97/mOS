#include "gdt.h"

#pragma pack(push, 1)
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
};

struct gdt_ptr {
    unsigned short limit;
    unsigned int   base;
};
#pragma pack(pop)

struct gdt_entry gdt[3];
struct gdt_ptr   gp;

extern void gdt_flush(unsigned int);

void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

int init_gdt() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base  = (unsigned int)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data

    gdt_flush((unsigned int)&gp);
    return 0; // Настройка успешна
}
