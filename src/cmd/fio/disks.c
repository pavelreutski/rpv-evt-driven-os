#include "disk.h"
#include "command.h"

#include "kernel_stdio.h"

static int disks_m(const int argc, const char **argv);

_SHELL_COMMAND(disks, disks_m);

static int disks_m(const int argc, const char **argv) {

    (void) argc;
    (void) argv;

	size_t n_disks = get_disks();

	for (size_t i = 0; i < n_disks; i++) {

		char dsk_info[255];
		get_diskInfo(i, dsk_info, sizeof(dsk_info));

		_kernel_outStringFormat("%s\n", dsk_info);		
	}

	return 0;
}