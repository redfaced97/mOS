#include "../kernel.h"

#include "../sys/ports.h"
#include "timer.h"

// Функция для таймера 0 (IRQ 0)

void timer_handler() {
    system_tick++;
    outb(0x20, 0x20);
}


void set_timer(unsigned long frequency) {
    // Расчёт делителя для таймера
    unsigned long divisor = 1193180 / frequency;

    // Установка режима меандра на таймер
    outb(0x43, 0x36);

    // Установка частоты
    outb(0x40, (unsigned char)(divisor & 0xFF));        // Младший байт
    outb(0x40, (unsigned char)((divisor >> 8) & 0xFF)); // Старший байт
}
