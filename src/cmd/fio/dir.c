#include "command.h"
#include "kernel_fsh.h"
#include "kernel_stdio.h"

static int ls_m(const int argc, const char **argv);

_SHELL_COMMAND(ls, ls_m);
_SHELL_COMMAND(dir, ls_m);

static int ls_m(const int argc, const char **argv) {

	_kernel_ls(argc, argv);
	return 0;
}
