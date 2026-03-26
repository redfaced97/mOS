#include "../kernel.h"
#include "../func/kernel_funcs.h"
#include "idt.h"
#include "../sys/ports.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

int init_idt(void) {
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    extern void generic_stub();
    for(int i=0;i<256;i++)
        idt_set_gate(i,(uint32_t)generic_stub,0x08,0x8E);

    extern void irq0_stub();
    extern void irq1_stub();
    idt_set_gate(32,(uint32_t)irq0_stub,0x08,0x8E);
    idt_set_gate(33,(uint32_t)irq1_stub,0x08,0x8E);

    __asm__ __volatile__("lidt %0"::"m"(idtp));

    // Remap PIC
    outb(0x20,0x11); outb(0xA0,0x11);
    outb(0x21,0x20); outb(0xA1,0x28);
    outb(0x21,0x04); outb(0xA1,0x02);
    outb(0x21,0x01); outb(0xA1,0x01);
    outb(0x21,0xFC);
    outb(0xA1,0xFF);

    return 0;
}

void generic_interrupt_handler(int int_no, int err) {
    _kpanic(int_no, err);
}