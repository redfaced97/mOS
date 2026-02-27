#include "../../sys/ports.h"
#include "video.h"

#define MAX_ANSI_PARAMS 4

static char *vram_color = (char*)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static short current_color = 0x0F;

// ANSI Parser State
static enum {
    STATE_NORMAL,
    STATE_ESC,
    STATE_CSI
} ansi_state = STATE_NORMAL;

static int ansi_params[MAX_ANSI_PARAMS];
static int ansi_arg_idx = 0;

short make_color(const short fore, const short back) {
    return (back << 4) | (fore & 0x0F);
}

void video_set_color(short fg, short bg) {
    current_color = make_color(fg, bg);
}

void video_cursor(int enable) {
    if (enable) {
        outb(VGA_CTRL, 0x0A);
        outb(VGA_DATA, (inb(VGA_DATA) & 0xC0) | 14);
        outb(VGA_CTRL, 0x0B);
        outb(VGA_DATA, (inb(VGA_DATA) & 0xE0) | 15);
    } else {
        outb(VGA_CTRL, 0x0A);
        outb(VGA_DATA, 0x20);
    }
}

void video_set_cursor(int x, int y) {
    int position = y * VGA_WIDTH + x;
    outb(VGA_CTRL, CURSOR_HIGH);
    outb(VGA_DATA, (position >> 8) & 0xFF);
    outb(VGA_CTRL, CURSOR_LOW);
    outb(VGA_DATA, position & 0xFF);
    cursor_x = x;
    cursor_y = y;
}

void video_clear(unsigned short bg) {
    unsigned char color_attr = (unsigned char)make_color(0x00, bg);
    unsigned int screen_size = VGA_WIDTH * VGA_HEIGHT;
    for (unsigned int i = 0; i < screen_size; i++) {
        vram_color[i * 2] = ' ';
        vram_color[i * 2 + 1] = color_attr;
    }
    video_set_cursor(0, 0);
    video_cursor(0);
}

void video_scroll() {
    int line_size = VGA_WIDTH * 2;
    for (int i = 0; i < (VGA_HEIGHT - 1) * line_size; i++) {
        vram_color[i] = vram_color[i + line_size];
    }
    int last_line_start = (VGA_HEIGHT - 1) * line_size;
    for (int i = 0; i < line_size; i += 2) {
        vram_color[last_line_start + i] = ' ';
        vram_color[last_line_start + i + 1] = (char)current_color;
    }
}

static void handle_sgr() {
    // Маппинг ANSI (0-7) в VGA (0-7)
    static const unsigned char ansi_to_vga[] = {
        0, // Black
        4, // Red
        2, // Green
        6, // Yellow
        1, // Blue
        5, // Magenta
        3, // Cyan
        7  // White
    };

    for (int i = 0; i <= ansi_arg_idx; i++) {
        int p = ansi_params[i];

        if (p == 0) {
            current_color = 0x0F; // Reset to White on Black
        }
        else if (p == 1) {
            // Делаем текущий цвет текста ярким (устанавливаем 3-й бит)
            current_color |= 0x08;
        }
        else if (p >= 30 && p <= 37) {
            // Обычный текст (30-37)
            int color_idx = p - 30;
            current_color = (current_color & 0xF0) | ansi_to_vga[color_idx];
        }
        else if (p >= 40 && p <= 47) {
            // Обычный фон (40-47)
            int color_idx = p - 40;
            current_color = (current_color & 0x0F) | (ansi_to_vga[color_idx] << 4);
        }
        else if (p >= 90 && p <= 97) {
            // Яркий текст (90-97) — сразу с битом интенсивности
            int color_idx = p - 90;
            current_color = (current_color & 0xF0) | ansi_to_vga[color_idx] | 0x08;
        }
        else if (p >= 100 && p <= 107) {
            // Яркий фон (100-107)
            int color_idx = p - 100;
            current_color = (current_color & 0x0F) | ((ansi_to_vga[color_idx] | 0x08) << 4);
        }
    }
}


void video_putchar(char c) {

    if (ansi_state == STATE_NORMAL) {
        if (c == 0x1B) { ansi_state = STATE_ESC; return; }
    } else if (ansi_state == STATE_ESC) {
        if (c == '[') {
            ansi_state = STATE_CSI;
            for(int i=0; i<MAX_ANSI_PARAMS; i++) ansi_params[i] = 0;
            ansi_arg_idx = 0;
        } else { ansi_state = STATE_NORMAL; }
        return;
    } else if (ansi_state == STATE_CSI) {
        if (c >= '0' && c <= '9') {
            ansi_params[ansi_arg_idx] = ansi_params[ansi_arg_idx] * 10 + (c - '0');
            return;
        } else if (c == ';') {
            if (ansi_arg_idx < MAX_ANSI_PARAMS - 1) ansi_arg_idx++;
            return;
        } else {
            if (c == 'm') handle_sgr();
            ansi_state = STATE_NORMAL;
            return;
        }
    }

    // --- Standard Character Logic ---
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + TAB_SIZE) & ~(TAB_SIZE - 1);
    } else if (c == '\b') {
        if (cursor_x > 0) cursor_x--;
        else if (cursor_y > 0) { cursor_y--; cursor_x = VGA_WIDTH - 1; }
        int offset = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        vram_color[offset] = ' ';
        vram_color[offset + 1] = (char)current_color;
    } else if (c != '\0') {
        int offset = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        vram_color[offset] = c;
        vram_color[offset + 1] = (char)current_color;
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) { cursor_x = 0; cursor_y++; }
    if (cursor_y >= VGA_HEIGHT) { video_scroll(); cursor_y = VGA_HEIGHT - 1; }
    video_set_cursor(cursor_x, cursor_y);
}

void kprint(const char *str) {
    while (*str) video_putchar(*str++);
}
