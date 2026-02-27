
#include "../kernel.h"
#include "../func/kernel_funcs.h"

#include "idt.h"
#include "../sys/ports.h"

void generic_interrupt_handler() {
    _kpanic(0);
}

#pragma pack(push, 1)
struct idt_entry {
    unsigned short base_low;
    unsigned short sel;
    unsigned char  always0;
    unsigned char  flags;
    unsigned short base_high;
};

struct idt_ptr {
    unsigned short limit;
    unsigned int   base;
};
#pragma pack(pop)

struct idt_entry idt[256];
struct idt_ptr   idtp;

void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

int init_idt() {
    // 1. Сначала подготавливаем структуру в памяти (прерывания еще выключены)
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;

    // 2. Сначала заполняем ВСЮ таблицу заглушками
    extern void generic_stub();
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, (unsigned int)generic_stub, 0x08, 0x8E);
    }

    // 3. Ставим конкретные обработчики
    extern void irq0_stub();
    extern void irq1_stub();
    idt_set_gate(32, (unsigned int)irq0_stub, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq1_stub, 0x08, 0x8E);

    // 4. Загружаем IDT в процессор
    __asm__ __volatile__("lidt %0" : : "m"(idtp));

    // 5. И ТОЛЬКО ТЕПЕРЬ настраиваем PIC
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28); // Ремаппинг
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);

    // Маскируем всё, кроме таймера и клавы
    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);

    // 6. Самый последний шаг - разрешаем прерывания
    return 0;
}

