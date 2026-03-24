#include "ata.h"
#include "../../sys/ports.h"

void ata_sector_read(unsigned int lba, unsigned short *buffer) {
    
    while (inb(0x1F7) & 0x80);

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));

    outb(0x1F2, 1);

    outb(0x1F3, (unsigned char)lba);         // Бит 0-7
    outb(0x1F4, (unsigned char)(lba >> 8));  // Бит 8-15
    outb(0x1F5, (unsigned char)(lba >> 16)); // Бит 16-23

    outb(0x1F7, 0x20);


    while ((inb(0x1F7) & 0x88) != 0x08);

    for (int i = 0; i < 256; i++) {
        buffer[i] = inw(0x1F0);
    }
}

void ata_sector_write(unsigned int lba, unsigned short *buffer) {
    while (inb(0x1F7) & 0x80);

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (unsigned char)lba);
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x30);

    while ((inb(0x1F7) & 0x88) != 0x08);

    for (int i = 0; i < 256; i++) {
        outw(0x1F0, buffer[i]);
    }

    outb(0x1F7, 0xE7);
    while (inb(0x1F7) & 0x80);
}


int ata_identify() {
    outb(0x1F6, 0xA0); 
    
    outb(0x1F2, 0);
    outb(0x1F3, 0);
    outb(0x1F4, 0);
    outb(0x1F5, 0);
    
    outb(0x1F7, 0xEC);

    unsigned char status = inb(0x1F7);
    if (status == 0) return 0;


    while (inb(0x1F7) & 0x80);
    

    if (inb(0x1F4) != 0 || inb(0x1F5) != 0) return 0;

    // Ждем готовности данных (DRQ)
    while (!(inb(0x1F7) & 0x08));

    unsigned short info[256];
    for (int i = 0; i < 256; i++) {
        info[i] = inw(0x1F0);
    }

    unsigned int sectors = *((unsigned int*)(info + 60));
    
    return 1;
}

