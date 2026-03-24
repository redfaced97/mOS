#ifndef FAT32_H
#define FAT32_H

typedef struct {
    unsigned char  boot_jmp[3];
    unsigned char  oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char  sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char  fat_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char  media_type;
    unsigned short sectors_per_fat_16;
    unsigned short sectors_per_track;
    unsigned short head_count;
    unsigned int   hidden_sectors;
    unsigned int   total_sectors_32;
    unsigned int   sectors_per_fat_32;
    unsigned short fat_flags;
    unsigned short fat_version;
    unsigned int   root_cluster;
    unsigned short fs_info_sector;
    unsigned short backup_boot_sector;
    unsigned char  reserved[12];
    unsigned char  drive_number;
    unsigned char  reserved1;
    unsigned char  boot_signature;
    unsigned int   volume_id;
    unsigned char  volume_label[11];
    unsigned char  system_id[8];
} __attribute__((packed)) fat32_bpb_t;

typedef struct {
    unsigned char  boot_indicator;
    unsigned char  starting_chs[3];
    unsigned char  os_type;
    unsigned char  ending_chs[3];
    unsigned int   starting_lba;
    unsigned int   size_in_sectors;
} __attribute__((packed)) mbr_partition_t;

typedef struct {
    unsigned char  signature[8];
    unsigned int   revision;
    unsigned int   header_size;
    unsigned int   header_crc32;
    unsigned int   reserved;
    unsigned long  current_lba;
    unsigned long  backup_lba;
    unsigned long  first_usable_lba;
    unsigned long  last_usable_lba;
    unsigned char  disk_guid[16];
    unsigned long  partition_entries_lba;
    unsigned int   num_partition_entries;
    unsigned int   partition_entry_size;
    unsigned int   partition_entries_crc32;
} __attribute__((packed)) gpt_header_t;

typedef struct {
    unsigned char  partition_type_guid[16];
    unsigned char  unique_partition_guid[16];
    unsigned long  starting_lba;
    unsigned long  ending_lba;
    unsigned long  attributes;
    unsigned short partition_name[36];
} __attribute__((packed)) gpt_entry_t;

typedef struct {
    unsigned char name[11];
    unsigned char attr;
    unsigned char nt_res;
    unsigned char crt_time_tenth;
    unsigned short crt_time;
    unsigned short crt_date;
    unsigned short lst_acc_date;
    unsigned short cluster_high;
    unsigned short wrt_time;
    unsigned short wrt_date;
    unsigned short cluster_low;
    unsigned int file_size;
} __attribute__((packed)) fat32_dir_t;

void fat32_load_bpb();
int fat32_init();
void fat32_list_root();
void fat32_read_file(const char* filename, unsigned char* target_buffer);

#endif
