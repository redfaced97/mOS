#include "kernel.h"

#include "sys/ports.h"
#include "timers/timer.h"

#include "sys/gdt.h"
#include "sys/idt.h"

#include "vga/video.h"
#include "keyboard/keyboard.h"
#include "func/kernel_funcs.h"

unsigned int system_tick = 0;

void app();


void kinit() {

    video_clear(VIDEO_COLOR_BLACK);
    video_set_color(VIDEO_COLOR_WHITE, VIDEO_COLOR_BLACK);

    if (init_gdt() == 0) {
      _klog("GDT ready!", 1);
    }


    if (init_idt() == 0) {
        _klog("IDT ready!", 1);
        return;
    }

    // 4. Только теперь настраиваем частоту таймера
    // Прерывания уже настроены в IDT, так что это безопасно
    set_timer(100);

    // 5. Переходим в оболочку
    app();
}

void app() {

    video_set_color(VIDEO_COLOR_WHITE, VIDEO_COLOR_BLACK);
    clear_input_buffer();
    kprint("\033[93mError: \033[32mSuccess \033[0mNormal\n");
    kprint("\nmOS kernel Shell ready. Type 'help'.\n> ");

    while(1) {
        if (command_ready) {
            if (kstrcmp(input_buffer, "help") == 0) {
                kprint("Commands: help, clear, test, system_tick, reboot\n");
            }
            else if (kstrcmp(input_buffer, "clear") == 0) {
                video_clear(VIDEO_COLOR_BLACK);
            }
            else if (kstrcmp(input_buffer, "system_tick") == 0) {
                kprint(int_to_char(system_tick));
                kprint("\n");
            }
            else if (kstrcmp(input_buffer, "reboot") == 0) {
                outb(0x64, 0xFE); // Команда перезагрузки ПК
            }
            else if (input_buffer[0] != '\0') {
                kprint("Unknown command: ");
                kprint(input_buffer);
                kprint("\n");
            }

            clear_input_buffer();
            if (kstrcmp(input_buffer, "clear") != 0) kprint("> ");
        }
        __asm__ __volatile__("hlt");
    }
    __asm__ __volatile__("hlt");
}
