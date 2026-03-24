#include "../kernel.h"

#include "kernel_funcs.h"
#include "../drivers/vga/video.h"


typedef __builtin_va_list va_list;
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)

static char format_buffer[32];

char* short_to_hex(unsigned short n) {
    static char hex_str[7];
    const char *hex_chars = "0123456789ABCDEF";

    hex_str[0] = '0';
    hex_str[1] = 'x';
    hex_str[2] = hex_chars[(n >> 12) & 0xF]; // Первые 4 бита
    hex_str[3] = hex_chars[(n >> 8) & 0xF];  // Вторые 4 бита
    hex_str[4] = hex_chars[(n >> 4) & 0xF];  // Третьи 4 бита
    hex_str[5] = hex_chars[n & 0xF];         // Последние 4 бита
    hex_str[6] = '\0';

    return hex_str;
}


static char* _num_to_str(long n, int is_signed) {
    unsigned long un = (unsigned long)n;
    char *ptr = &format_buffer[31];
    *ptr = '\0';

    int negative = 0;
    if (is_signed && n < 0) {
        negative = 1;
        un = (unsigned long)(-n);
    }

    if (un == 0) {
        *(--ptr) = '0';
    } else {
        while (un > 0) {
            *(--ptr) = (un % 10) + '0';
            un /= 10;
        }
    }

    if (negative) *(--ptr) = '-';
    return ptr;
}

char* int_to_char(int n)            { return _num_to_str((long)n, 1); }
char* long_to_char(long n)          { return _num_to_str(n, 1);       }
char* short_to_char(short n)        { return _num_to_str((long)n, 1); }

char* uint_to_char(unsigned int n)  { return _num_to_str((long)n, 0); }
char* ulong_to_char(unsigned long n){ return _num_to_str((long)n, 0); }
char* ushort_to_char(unsigned short n){ return _num_to_str((long)n, 0); }

char* float_to_char(float n, int precision) {
    static char f_buffer[64];
    char *ptr = f_buffer;

    if (n < 0) {
        *ptr++ = '-';
        n = -n;
    }

    long ipart = (long)n;
    float fpart = n - (float)ipart;
    char* s_ipart = long_to_char(ipart);
    while(*s_ipart) *ptr++ = *s_ipart++;

    if (precision > 0) {
        *ptr++ = '.';
        for(int i = 0; i < precision; i++) fpart *= 10;

        long ifpart = (long)(fpart + 0.5f);
        char* s_fpart = long_to_char(ifpart);
        int len = 0;
        for(char* t = s_fpart; *t; t++) len++;
        for(int i = 0; i < (precision - len); i++) *ptr++ = '0';

        while(*s_fpart) *ptr++ = *s_fpart++;
    }

    *ptr = '\0';
    return f_buffer;
}


void _kpanic(int error_code) {

    __asm__ __volatile__("cli");

    // 0-9   - Cистемные ошибки
    // 10-20 - Пользовательские ошибки

    const char* error_msg = "";

    if(error_code == 0) {
      error_msg = "UNHANDLED INTERRUPT!";
    }


    video_clear(VIDEO_COLOR_BLACK);
    video_set_color(VIDEO_COLOR_YELLOW, VIDEO_COLOR_BLACK);


    kprint("--//   CRITICAL ERROR!\n");
    kprint("INFO - KERNEL PANIC\n\n");
    if(error_code <= 9) {
      video_set_color(VIDEO_COLOR_YELLOW, VIDEO_COLOR_BLACK);
    }
    if(error_code >= 10) {
      video_set_color(VIDEO_COLOR_LIGHT_GREEN, VIDEO_COLOR_BLACK);
    }
    kprint("\nError code - "); kprint(int_to_char(error_code)); kprint("\nMESSAGE - "); kprint(error_msg);
    video_set_color(VIDEO_COLOR_YELLOW, VIDEO_COLOR_BLACK);
    kprint("\n");
    kprint(int_to_char(system_tick));
    kprint("\n\n\n--//   Please reboot computer to manually!\n");

    video_cursor(0);

    while(1) {
        __asm__ __volatile__("hlt");
    }
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p == '%' && *(p + 1) != '\0') {
            p++;
            switch (*p) {
                case 'd':
                    kprint(int_to_char(va_arg(args, int)));
                    break;
                case 'l':
                    kprint(long_to_char(va_arg(args, long)));
                    break;
                case 's': {
                    char* s = va_arg(args, char*);
                    kprint(s ? s : "(null)");
                    break;
                }
                case 'f':
                    kprint(float_to_char((float)va_arg(args, double), 2));
                    break;
                case 'c':
                    video_putchar((char)va_arg(args, int));
                    break;
                case '%':
                    video_putchar('%');
                    break;
                default:
                    video_putchar('%');
                    video_putchar(*p);
                    break;
            }
        } else {
            video_putchar(*p);
        }
    }
    va_end(args);
}

void _klog(const char *str, int status) {

    switch (status) {
        case 0: kprintf(" \033[37m[  \033[92mOK\033[37m  ]\033[0m %s\n", str); break;
        case 1: kprintf(" \033[37m[ \033[92mINFO\033[37m ]\033[0m %s\n", str); break;
        case 2: kprintf(" \033[37m[ \033[93mWARN\033[37m ]\033[0m %s\n", str); break;
        case 3: kprintf(" \033[37m[\033[91mFAILED\033[37m]\033[0m %s\n", str); break;
        default: kprintf(" [ LOG  ] %s\n", str); break;
    }
}

int kstrcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}


void __stack_chk_fail(void) {
    _kpanic(0);
}

void __stack_chk_fail_local(void) {
    __stack_chk_fail();
}
