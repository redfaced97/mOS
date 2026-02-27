#ifndef VIDEO_H
#define VIDEO_H

#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5
#define CURSOR_HIGH 0x0E
#define CURSOR_LOW 0x0F

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_CHAR_WIDHT 2

/* Стандартные цвета (0-7) */
#define VIDEO_COLOR_BLACK         0x00
#define VIDEO_COLOR_BLUE          0x01
#define VIDEO_COLOR_GREEN         0x02
#define VIDEO_COLOR_CYAN          0x03
#define VIDEO_COLOR_RED           0x04
#define VIDEO_COLOR_MAGENTA       0x05
#define VIDEO_COLOR_BROWN         0x06
#define VIDEO_COLOR_LIGHT_GREY    0x07

#define VIDEO_COLOR_DARK_GREY     0x08
#define VIDEO_COLOR_LIGHT_BLUE    0x09
#define VIDEO_COLOR_LIGHT_GREEN   0x0A
#define VIDEO_COLOR_LIGHT_CYAN    0x0B
#define VIDEO_COLOR_LIGHT_RED     0x0C
#define VIDEO_COLOR_LIGHT_MAGENTA 0x0D
#define VIDEO_COLOR_YELLOW        0x0E
#define VIDEO_COLOR_WHITE         0x0F

#define TAB_SIZE 4

short make_color(const short fore, const short back);

// Прототипы (говорим компилятору, что функции есть где-то еще)
void video_clear(unsigned short bg);
void video_set_cursor(int x, int y);
void video_set_color(short fg, short bg);
void video_scroll();
void video_cursor(int enable);
void video_putchar(char c);
void kprint(const char *str);

#endif