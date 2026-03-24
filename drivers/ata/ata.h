#ifndef ATA_H
#define ATA_H

#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SEC_COUNT   0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_SEL   0x1F6
#define ATA_CMD         0x1F7
#define ATA_STATUS      0x1F7

#define ATA_CMD_READ    0x20
#define ATA_STATUS_BUSY 0x80
#define ATA_STATUS_DRQ  0x08

int ata_identify();
void ata_sector_read(unsigned int lba, unsigned short *buffer);
void ata_sector_write(unsigned int lba, unsigned short *buffer);

#endif
