#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "command.h"

#include "fat.h"
#include "kernel_stdio.h"

#define FILE_BUFFER                                 (2048)

#define CON_P                                       ("con")

#define FILE_NO_DATA_TO_SAVE_MSG                    ("Nothing to save\n")
#define FILE_SAVED_MSG                              ("File saved successfully\n")

#define FILE_WRITE_FAILED_MSG                       ("write failed")

#define FILE_OPEN_READ_FAILED_MSG                   ("unable open file to read")
#define FILE_OPEN_WRITE_FAILED_MSG                  ("unable open file for write")

#define NO_SUCH_FILE_MSG                            ("No such file")
#define TOO_FEW_PARAMS_MSG                          ("Too few parameters specified\n")

#define FILE_EXISTS_MSG                             ("Console out to file error: file exists\n")

#define CON_TO_FILE_COPY_WELCOME_MSG                ("Console out to file is opened => Text being input will be saved to the file\n")
#define CON_TO_FILE_COPY_HINT_MSG                   ("Press keys: F1 to save, F2 to display this message again and ESC to quit...\n")

static int con_toFile(char const* path);

static void close_fdesc(int sfd, int dfd);
static bool flush_conFile(int fd, char const* buffer, size_t *len);

static int cp_m(const int argc, const char **argv);

_SHELL_COMMAND(cp, cp_m);
_SHELL_COMMAND(copy, cp_m);

static int cp_m(const int argc, const char **argv) {

    if (argc < 3) {

        _kernel_outString(TOO_FEW_PARAMS_MSG);
        return -1;
    }

    if (strcmp(argv[1], CON_P) == 0) { // copy con [file]
        return con_toFile(argv[2]);
    }

    char *src_p = (char *) argv[1];
    char *dst_p = (char *) argv[2];

    if (!fat_exists(src_p)) {

        _kernel_outStringFormat("source: %s\n", NO_SUCH_FILE_MSG);
        return -1;
    }

    int f_cp = 0;
    int sfd = -1, dfd = -1;
    if ((sfd = fat_fopen(src_p, FILE_READ)) < 0) {

        _kernel_outStringFormat("source: %s\n", FILE_OPEN_READ_FAILED_MSG);
        return -1;
    }

    if ((dfd = fat_fopen(dst_p, (FILE_CREATE | FILE_WRITE))) < 0) {

        close_fdesc(sfd, dfd);
        _kernel_outStringFormat("dest: %s\n", FILE_OPEN_WRITE_FAILED_MSG);

        return -1;
    }

    size_t b_read;
    uint8_t f_buffer[FILE_BUFFER];

    while((b_read = fat_fread(sfd, f_buffer, sizeof(f_buffer))) > 0) {

        if (fat_fwrite(dfd, f_buffer, b_read) != b_read) {

            close_fdesc(sfd, dfd);
            _kernel_outStringFormat("dest: %s\n", FILE_WRITE_FAILED_MSG);

            return -1;
        }
    }

    f_cp = 1;
    _kernel_outStringFormat("%d file(s) copied\n", f_cp);

    close_fdesc(sfd, dfd);
    return 0;
}

static __attribute__((noinline)) void close_fdesc(int sfd, int dfd) {

    fat_fclose(sfd);
    fat_fclose(dfd);
}

static __attribute__((noinline)) bool flush_conFile(int fd, char const* buffer, size_t *len) {

    size_t l = *len;

    if (l == 0) {
        return true;
    }

    bool write_Ok = (fat_fwrite(fd, buffer, l) == l);

    _kernel_outLn();

    if (!write_Ok) {
        _kernel_outString(FILE_WRITE_FAILED_MSG);
    } else {

        *len = 0;      
        _kernel_outString(FILE_SAVED_MSG);
    }

    return write_Ok;
}

static __attribute__((noinline)) int con_toFile(char const* path) {

    if (fat_exists(path)){

        _kernel_outString(FILE_EXISTS_MSG);
        return -1;
    }

    int fd;

    if ((fd = fat_fopen(path, FILE_CREATE | FILE_WRITE)) < 0) {

        _kernel_outString(FILE_WRITE_FAILED_MSG);
        return -1;
    }

    _kernel_outString(CON_TO_FILE_COPY_WELCOME_MSG);
    _kernel_outString(CON_TO_FILE_COPY_HINT_MSG);

    size_t len = 0;
    int ret_code = 0;

    bool input_loop = true;

    char buffer[FILE_BUFFER];

    do {

        if (len == sizeof(buffer)) {

            if (!flush_conFile(fd, buffer, &len)) {
                
                ret_code = -1;
                input_loop = false;

                continue;
            }
        }

        console_key_t key;
        _kernel_getKey(&key);

        switch (key.scan_code) {

            case 0x76: { // ESC

                input_loop = false;

                if (!flush_conFile(fd, buffer, &len)) {
                    ret_code = -1;
                }

            } break;

            case 0x05: { // F1

                if (len == 0) {

                    _kernel_outLn();
                    _kernel_outString(FILE_NO_DATA_TO_SAVE_MSG);
                    break;
                }

                if (!flush_conFile(fd, buffer, &len)) {

                    ret_code = -1;
                    input_loop = false;                    
                }

            } break;

            case 0x06: { // F2
                _kernel_outLn();
                _kernel_outString(CON_TO_FILE_COPY_HINT_MSG);
            } break;
        
            default: {

                if (key.code == '\0') {
                    break;
                }

                switch (key.code) {

                    case '\b': { // backspace

                        if (len == 0) { 
                            continue; 
                        }

                        len--;

                    } break;
                
                    default: {
                        buffer[len++] = key.code;
                    } break;
                }

                _kernel_outChar(key.code);

            } break;
        }

    } while(input_loop);

    close_fdesc(-1, fd);
    return ret_code;
}