
#include "kernel_setup.h"
#include "kernel.h"


volatile unsigned int system_tick = 0;
volatile unsigned int system_sec  = 0;

void app();


void kinit() {

    video_clear(VIDEO_COLOR_BLACK);
    video_set_color(VIDEO_COLOR_WHITE, VIDEO_COLOR_BLACK);

    if (init_gdt() == 0) {
      _klog("GDT ready!", 0);
    }


    if (init_idt() == 0) {
        _klog("IDT ready!", 0);
    }


    // if (ata_identify() == 1) {
    //     _klog("ATA HDD ready!", 0);
    //     if (fat32_init() == 1) {
    //         fat32_load_bpb();
    //         //fat32_list_root();
    //     }
    // }



    if(IRQ0_set_freq(KERNEL_IRQ0_FREQ) == 0) {
        _klog("IRQ0 (timer) set!", 0);
    }


    __asm__ __volatile__("sti");

    if (fat32_init() == 1) {
        fat32_load_bpb();
        fat32_list_root();
    }
    app();
    __asm__ __volatile__("hlt");
}



void app() {

    video_set_color(VIDEO_COLOR_WHITE, VIDEO_COLOR_BLACK);
    clear_input_buffer();

    static unsigned char file_buffer[512];
    for(int i = 0; i < 512; i++) file_buffer[i] = 0;

    fat32_read_file("TEST    TXT", (char*)file_buffer);

    kprint("DATA: ");
    kprint((char*)file_buffer);
    kprint("\n");

    kprint("\n");
    kprint("\nmOS kernel Shell ready. Type 'help'.\n> ");

    while(1) {
        if (command_ready) {
            if (kstrcmp(input_buffer, "help") == 0) {
                kprint("Commands: help, clear, system_tick, uptime, reboot\n");
            }
            else if(kstrcmp(input_buffer, "clear") == 0) {
                video_clear(VIDEO_COLOR_BLACK);
            }
            else if(kstrcmp(input_buffer, "system_tick") == 0) {
                kprint(int_to_char(system_tick));
                kprint("\n");
            }
            else if(kstrcmp(input_buffer, "reboot") == 0) {
                outb(0x64, 0xFE);
            }
            else if(kstrcmp(input_buffer, "uptime") == 0) {
                unsigned int seconds = system_sec % 60;          //  секунд
                unsigned int minutes = (system_sec / 60) % 60;   //  минут
                unsigned int hours   = (system_sec / 3600) % 24; //  часов
                unsigned int days    = system_sec / 86400;       //  сутки

                kprint(int_to_char(days));
                kprint("\n");
                kprint(int_to_char(hours));
                kprint("\n");
                kprint(int_to_char(minutes));
                kprint("\n");
                kprint(int_to_char(seconds));
                kprint("\n");

            }

            else if(kstrcmp(input_buffer, "read_ata") == 0) {
                unsigned short sector_data[256];
                ata_sector_read(0, sector_data);

                for(int i = 0; i < 256; i++) {
                    kprint(short_to_hex(sector_data[i]));
                    kprint(" ");

                    if ((i + 1) % 8 == 0) kprint("\n");
                }
            }


            else if(input_buffer[0] != '\0') {
                kprint("Unknown command: ");
                kprint(input_buffer);
                kprint("\n");
            }

            clear_input_buffer();
            if (kstrcmp(input_buffer, "clear") != 0) kprint("> ");
        }
        __asm__ __volatile__("hlt");
    }
}
