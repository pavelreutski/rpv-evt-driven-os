#include "command.h"

#include "kernel_fsh.h"
#include "kernel_stdio.h"

static int cd_m(int argc, const char **argv);

_SHELL_COMMAND(cd, cd_m);

static int cd_m(const int argc, const char **argv) {

    if (argc < 2) {

        _kernel_outString("no directory given\n");
        return -1;
    }

	_kernel_cd(argv[1]);
	return 0;
}
