#include "version.h"
#include "command.h"

#include "kernel_stdio.h"

static int ver_m(const int argc, const char **argv);

_SHELL_COMMAND(ver, ver_m);

static int ver_m(const int argc, const char **argv) {

	(void) argc;
    (void) argv;

	_kernel_outStringFormat(
		"Event Driven OS [Version %d.%d.%d-%s]\n", 
			MAJOR, MINOR, PATCH, REVISION);

	return 0;
}