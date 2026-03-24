#include "fat32.h"
#include "../../ata/ata.h"
#include "../../../func/kernel_funcs.h"
#include "../../vga/video.h"

unsigned int fat32_lba_offset = 0;

static fat32_bpb_t bpb;
static unsigned int fat_lba;
static unsigned int data_lba;


void fat32_load_bpb() {
    unsigned short buffer[256];
    ata_sector_read(fat32_lba_offset, buffer);

    unsigned char* src = (unsigned char*)buffer;
    unsigned char* dst = (unsigned char*)&bpb;
    for(int i = 0; i < sizeof(fat32_bpb_t); i++) dst[i] = src[i];

    fat_lba = fat32_lba_offset + bpb.reserved_sectors;
    data_lba = fat_lba + (bpb.fat_count * bpb.sectors_per_fat_32);
}

unsigned int cluster_to_lba(unsigned int cluster) {
    return data_lba + ((cluster - 2) * bpb.sectors_per_cluster);
}

unsigned int get_next_cluster(unsigned int cluster) {
    unsigned short buffer[256];
    unsigned int fat_sector = fat_lba + (cluster * 4 / 512);
    unsigned int fat_offset = (cluster * 4) % 512;

    ata_sector_read(fat_sector, buffer);
    unsigned int* fat_table = (unsigned int*)buffer;
    return fat_table[fat_offset / 4] & 0x0FFFFFFF;
}

int fat32_init() {
    unsigned short buffer[256];
    ata_sector_read(0, buffer);

    unsigned char* raw = (unsigned char*)buffer;


    if (raw[510] != 0x55 || raw[511] != 0xAA) {
        return 0;
    }

    if (raw[0x52] == 'F' && raw[0x53] == 'A' && raw[0x54] == 'T') {
        fat32_lba_offset = 0;
        return 1;
    }

    mbr_partition_t* mbr = (mbr_partition_t*)(raw + 0x1BE);
    for (int i = 0; i < 4; i++) {

        if (mbr[i].os_type == 0xEE) {
            ata_sector_read(1, buffer);
            gpt_header_t* gpt = (gpt_header_t*)buffer;

            if (gpt->signature[0] == 'E' && gpt->signature[1] == 'F' && gpt->signature[2] == 'I') {
                unsigned int entry_lba = (unsigned int)gpt->partition_entries_lba;
                ata_sector_read(entry_lba, buffer);
                gpt_entry_t* entry = (gpt_entry_t*)buffer;
                fat32_lba_offset = (unsigned int)entry->starting_lba;
                return 1;
            }
        }

        if (mbr[i].os_type == 0x0C || mbr[i].os_type == 0x0B) {
            fat32_lba_offset = mbr[i].starting_lba;
            return 1;
        }
    }
    return 0;
}



void fat32_list_root() {
    unsigned short buffer[256];
    unsigned int current_cluster = bpb.root_cluster;


    while (current_cluster < 0x0FFFFFF8) {
        unsigned int lba = cluster_to_lba(current_cluster);

        for (int s = 0; s < bpb.sectors_per_cluster; s++) {
            ata_sector_read(lba + s, buffer);
            fat32_dir_t* dir = (fat32_dir_t*)buffer;

            for (int i = 0; i < 16; i++) {
                if (dir[i].name[0] == 0x00) {
                    kprint("FAT32: End of directory\n");
                    return;
                }
                if (dir[i].name[0] == 0xE5) continue;
                if (dir[i].attr == 0x0F) continue;

                char name[13];
                for(int j = 0; j < 11; j++) name[j] = dir[i].name[j];
                name[11] = ' ';
                name[12] = '\0';

                kprint("- ");
                kprint(name);
                if (dir[i].attr & 0x10) kprint(" <DIR>");
                else kprint(int_to_char(dir[i].file_size));
                kprint(" bytes\n");
            }
        }
        current_cluster = get_next_cluster(current_cluster);
        if (current_cluster == 0) break;
    }
}



void fat32_read_file(const char* filename, unsigned char* target_buffer) {
    unsigned short sector_buffer[256];
    unsigned int current_cluster = bpb.root_cluster;
    fat32_dir_t target_entry;
    int found = 0;

    while (current_cluster < 0x0FFFFFF8) {
        unsigned int lba = cluster_to_lba(current_cluster);

        for (int s = 0; s < bpb.sectors_per_cluster; s++) {
            ata_sector_read(lba + s, sector_buffer);
            fat32_dir_t* dir = (fat32_dir_t*)sector_buffer;

            for (int i = 0; i < 16; i++) {
                if (dir[i].name[0] == 0x00) break;

                char entry_name[12];
                for(int j = 0; j < 11; j++) entry_name[j] = dir[i].name[j];
                entry_name[11] = '\0';

                if (kstrcmp(entry_name, filename) == 0) {
                    target_entry = dir[i];
                    found = 1;
                    break;
                }
            }
            if (found) break;
        }
        if (found) break;
        current_cluster = get_next_cluster(current_cluster);
    }

    if (!found) return;

    unsigned int file_cluster = ((unsigned int)target_entry.cluster_high << 16) | target_entry.cluster_low;
    unsigned int bytes_read = 0;

    while (file_cluster < 0x0FFFFFF8) {
        unsigned int file_lba = cluster_to_lba(file_cluster);

        for (int s = 0; s < bpb.sectors_per_cluster; s++) {
            if (bytes_read >= target_entry.file_size) break;

            ata_sector_read(file_lba + s, sector_buffer);

            unsigned char* ptr = (unsigned char*)sector_buffer;
            for (int b = 0; b < 512; b++) {
                if (bytes_read < target_entry.file_size) {
                    target_buffer[bytes_read++] = ptr[b];
                }
            }
        }

        file_cluster = get_next_cluster(file_cluster);
    }
}
