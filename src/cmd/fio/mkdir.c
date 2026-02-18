#include "command.h"

#include "kernel_fsh.h"
#include "kernel_stdio.h"

static int mkdir_m(const int argc, const char **argv);

_SHELL_COMMAND(mkdir, mkdir_m);

static int mkdir_m(const int argc, const char **argv) {

	if (argc < 2) {

		_kernel_outString("no directory name given\n");
		return -1;
	}

	_kernel_mkdir(argv[1]);
	return 0;
}