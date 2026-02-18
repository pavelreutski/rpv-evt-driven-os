#include "console.h"
#include "command.h"

static int cls_m(const int argc, const char **argv);

_SHELL_COMMAND(cls, cls_m);

static int cls_m(const int argc, const char **argv) {

	(void) argc;
    (void) argv;

	con_clear();
	return 0;
}