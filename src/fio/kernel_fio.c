#include <stddef.h>
#include <string.h>

#include "fat.h"
#include "disk.h"

#include "kernel_fio.h"
#include "kernel_jrnl.h"

#define MAX_FIO_DISKS   10

static volatile disk_t *c_dsk = NULL;
static volatile disk_t fio_disks[MAX_FIO_DISKS];

void _kernel_fio(void) {

    disk_io();

    size_t if_disks = get_disks();

    memset((void *) fio_disks, 0, sizeof(fio_disks));

    for (size_t did = 0, f_disk = 0; 
            (did < if_disks) && (f_disk < MAX_FIO_DISKS); did++) {

        disk_t disk;
        get_disk(did, &disk);

        if (disk.letter != '\0') {

            memcpy((void*) &fio_disks[f_disk], &disk, sizeof(disk_t));

            /* just a first drive presented as a current disk.
               should be replaced to a bootable drive later */

            if (c_dsk == NULL) {

                char mnt[] = { disk.letter, ':', '\0' };

                if (fat_mount(mnt)) {
                    c_dsk = &fio_disks[f_disk];
                }
            }

            f_disk++;
        }
    }
}

char _kernel_cdrive(void) {
    return c_dsk != NULL ? c_dsk -> letter : '\0';
}

bool _kernel_unmount(void) {

    if (c_dsk == NULL) {
        return false;
    }

    char mnt[] = { c_dsk -> letter, ':', '\0' };

    if (fat_unmount(mnt)) {

        _kernel_jentry("[kernel] info: path '%s' unmount success", mnt);

        c_dsk = NULL;
        return true;
    }

    return false;
}

bool _kernel_mount(char letter) {

    disk_t *fio_dsk = (disk_t *) fio_disks;

    for (size_t i = 0; 
            (fio_dsk -> letter) != '\0' && (i < MAX_FIO_DISKS); fio_dsk++, i++) {

        if (fio_dsk -> letter == letter) {

            char mnt[] = { letter, ':', '\0' };
            if (fat_mount(mnt)) {                

                _kernel_jentry("[kernel] info: path '%s' mount success", mnt);

                c_dsk = fio_dsk;
                return true;
            }
        }
    }

    return false;
}

size_t _kernel_drives(char *drive_lt, size_t nlt) {

    if (nlt >= MAX_FIO_DISKS) return 0;

    char *ltrs = drive_lt;
    size_t n_ltrs = 0;

    for (volatile disk_t *fio_disk = fio_disks; 
        (fio_disk -> letter) != '\0'; fio_disk++, n_ltrs++) {
            *ltrs = fio_disk -> letter;
    }

    return n_ltrs;
}
