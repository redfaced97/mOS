#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#pragma pack(push, 1)
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
};

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
};
#pragma pack(pop)

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
int init_idt(void);
void generic_interrupt_handler(int int_no, int err);

#endif