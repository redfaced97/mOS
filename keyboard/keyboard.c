#include "../sys/ports.h"
#include "../vga/video.h"
#include "keyboard.h"

char input_buffer[INPUT_BUFFER_SIZE];

int buffer_idx = 0;
int command_ready = 0;


static int shift = 0;      // Флаг шифта
static int caps_lock = 0;  // Флаг капслока

// Список клавиш без модификатора
unsigned char kbd_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0
};

// Список клавиш при модификаторе
unsigned char kbd_map_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0
};

// Сравнивание строк
int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// Очистка входного буффера
void clear_input_buffer() {
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++) input_buffer[i] = 0;
    buffer_idx = 0;
    command_ready = 0;
}

void keyboard_handler_main() {
    unsigned char scancode = inb(0x60);

    // Обработка отпускания клавиш
    if (scancode & 0x80) {
        unsigned char released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) shift = 0;
    }
    // Логика работы клавы
    else {
        if (scancode == 0x2A || scancode == 0x36) {
            shift = 1;
        } else if (scancode == 0x3A) {
            caps_lock = !caps_lock;
        } else if (scancode == 0x0E) {        // Backspace
            if (buffer_idx > 0) {
                buffer_idx--;
                input_buffer[buffer_idx] = 0;
                video_putchar('\b');
            }
        } else if (scancode == 0x1C) {        // Enter
            input_buffer[buffer_idx] = '\0';
            video_putchar('\n');
            command_ready = 1;
        } else {
            char c = shift ? kbd_map_shift[scancode] : kbd_map[scancode];

            // Проверка на капс-лок
            if (caps_lock) {
                if (c >= 'a' && c <= 'z') c -= 32;
                else if (c >= 'A' && c <= 'Z') c += 32;
            }

            if (c > 0 && buffer_idx < INPUT_BUFFER_SIZE - 1) {
                input_buffer[buffer_idx++] = c;
                video_putchar(c);
            }
        }
    }
    outb(0x20, 0x20);   //Сигнал завершения прерывания
}
