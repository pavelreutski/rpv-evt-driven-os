#include <string.h>

#include "fat.h"
#include "console.h"
#include "monitor.h"

#include "command.h"

#include "kernel_stdio.h"

#define MAX_FILE_VIEW_BUFFER                           1024

#define NO_SUCH_FILE_MSG                               ("No such file\n")
#define TOO_FEW_PARAMS_MSG                             ("File to open is not specified\n")

enum {
    FILE_HEXVIEW,
    FILE_TEXTVIEW
};

static int type_m(const int argc, const char **argv);

_SHELL_COMMAND(cat, type_m);
_SHELL_COMMAND(type, type_m);

static int type_m(const int argc, const char **argv) {

    if (argc == 0) {

        _kernel_outString(TOO_FEW_PARAMS_MSG);
        return -1;
    }

    char *path = (char *)((argc > 2) ? argv[2] : argv[1]);

    uint8_t fv = 
        (argc > 2 && !strcmp(argv[1], "hex")) ? FILE_HEXVIEW : FILE_TEXTVIEW;

    if (!fat_exists(path)) {

        _kernel_outString(NO_SUCH_FILE_MSG);
        return -1;
    }
    
    int fd = fat_fopen(path, FILE_READ);

    size_t b_read;
    char buffer[MAX_FILE_VIEW_BUFFER];

    while ((b_read = fat_fread(fd, buffer, sizeof(buffer) - 1)) > 0) {

        switch(fv) {

            case FILE_HEXVIEW: {
                
                con_clear();
                hex_monitor(buffer, b_read);
            } break;

            case FILE_TEXTVIEW: {

                buffer[b_read] = '\0';
                text_monitor(buffer);
            }
        }
    }
        
    fat_fclose(fd);
    _kernel_outChar('\n');

    return 0;
}