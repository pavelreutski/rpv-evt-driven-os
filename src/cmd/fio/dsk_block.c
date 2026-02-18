#include <stdlib.h>

#include "disk.h"
#include "monitor.h"
#include "console.h"
#include "command.h"

#include "kernel_stdio.h"

static int dsk_blockm(const int argc, const char **argv);

_SHELL_COMMAND(dsk_block, dsk_blockm);

static int dsk_blockm(const int argc, const char **argv) {    

	if (argc != 3) {

		_kernel_outString("Invalid parameters. Disk id and block numer (zero based) is required\n");
		return -1;
	}
	
	uint8_t blck_data[512];

	size_t did = atol(argv[1]);
	size_t blck = atol(argv[2]);

	uint8_t disk_io = read_disk(blck_data, did, blck, 1);

	switch (disk_io) {

		case DISK_INVALID: { _kernel_outString("drive invalid !\n"); } break;
		case DISK_NOT_READY: { _kernel_outString("drive not ready !\n"); } break;
 		case DISK_IO_ERR: { _kernel_outString("drive read block failed !\n"); } break;
	}

	if (disk_io != DISK_IO_OK) {
		return -1;
	}

	con_clear();
	hex_monitor(blck_data, sizeof(blck_data));

	return 0;
}