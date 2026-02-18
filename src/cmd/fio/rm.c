#include "command.h"

#include "kernel_fsh.h"
#include "kernel_stdio.h"

static int rmfile_m(const int argc, const char **argv);

_SHELL_COMMAND(rm, rmfile_m);
_SHELL_COMMAND(rmdisk, rmfile_m);

static int rmfile_m(const int argc, const char **argv) {

	if (argc < 2) {

		_kernel_outString("no file name given\n");
		return -1;
	}

	_kernel_rmfile(argv[1]);
	return 0;
}