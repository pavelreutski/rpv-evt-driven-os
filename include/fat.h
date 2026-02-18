#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_FILE_NAME           (256)

typedef enum {

    FILE_READ              = 0x01, /* permission for write */
    FILE_WRITE             = 0x02, /* permission for read */
    FILE_CREATE            = 0x08, /* creats a new file: overwrites old file if exists */
    FILE_CREATE_NEW        = 0x04, /* creates a new file: does not overwrite old file if exists instead an open error is given */
    FILE_APPEND            = 0x30  /* opens a file: sets an I/O cursor at the end of a file */

} file_access_t;

typedef enum {

    FAT_OK,
    FAT_IO_ERROR,
    FAT_NOT_FOUND,
    FAT_ACCESS_DENIED,
    FAT_ERR_NOTSET

} fat_error_code_t;

typedef struct {

    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t day;
    uint8_t month;
    uint16_t year;

} fildatetime_t;

typedef struct {

    fildatetime_t create_time;
    fildatetime_t modify_time;

    union {
        uint8_t a8;
        struct {
            bool r_only    : 1;
            bool archive   : 1;
            bool system    : 1;
            bool hidden    : 1;
            bool directory : 1;
        } flags;

    } attrib;    

    size_t filesize;

    char full_name[MAX_FILE_NAME];

} filinfo_t;

void fat_getcwd(char *cwd);

bool fat_exists(char const* path);

bool fat_chdir(char const* cwd);
bool fat_mkdir(char const* path);
bool fat_unlink(char const* path);

bool fat_mount(char const* path);
bool fat_unmount(char const* path);

bool fat_stat(char const* path, filinfo_t *fno);

fat_error_code_t fat_getcode(void);

void fat_fclose(int fd);
int fat_fopen(char const* path, file_access_t mode);

size_t fat_fsize(int fd);
size_t fat_fread(int fd, void *buffer, size_t nread);
size_t fat_fwrite(int fd, void const* buffer, size_t nwrite);

void fat_ls(char const* path, void const* ls_ctx, 
    void (*nxt_file)(void const* ls_ctx, filinfo_t const* fi));