#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum {

    DISK_IO_OK,
    DISK_IO_ERR,
    DISK_INVALID,
    DISK_NOT_READY,
    DISK_WRITE_PROTECT
};

typedef struct {

    char letter;
    
    size_t did;
    size_t volume;
    size_t num_blocks;
    size_t block_size;    

} disk_t;

void disk_io(void);

size_t get_disks(void);

void get_disk(size_t did, disk_t *disk);
void get_diskInfo(size_t did, char *dsk_info, size_t dsk_infoLen);

uint8_t flush_disk(size_t did);

uint8_t read_disk(void *buffer, size_t did, size_t blck, size_t blocks);
uint8_t write_disk(void const* buffer, size_t did, size_t blck, size_t blocks);
