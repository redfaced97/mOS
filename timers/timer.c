#include "../kernel.h"

#include "../sys/ports.h"
#include "timer.h"

// Функция для таймера 0 (IRQ 0)

void IRQ0_handler() {
    system_tick++;
    system_sec = system_tick / 100;
    outb(0x20, 0x20);
}


int IRQ0_set_freq(unsigned long frequency) {
    unsigned long divisor = 1193180 / frequency;

    outb(0x43, 0x36); // Пульсы

    // Установка частоты
    outb(0x40, (unsigned char)(divisor & 0xFF));        // Младший байт
    outb(0x40, (unsigned char)((divisor >> 8) & 0xFF)); // Старший байт

    return 0;
}
