#include <stdlib.h>

#include "monitor.h"
#include "console.h"
#include "command.h"

#include "kernel_stdio.h"

static int rmon_m(const int argc, const char **argv);

_SHELL_COMMAND(rmon, rmon_m);

static int rmon_m(const int argc, const char **argv) {

    if (argc < 3) {

        _kernel_outString("mem addr and size are required\n");
        return -1;
    }

    size_t m_len = atol(argv[2]);

    char *e_arg;
    size_t m_addr = strtoul(argv[1], &e_arg, 16);

    if (m_len == 0) {

        _kernel_outString("mem size invalid\n");
        return -1;
    }

    if ((*e_arg) != '\0') {

        _kernel_outString("mem addr invalid\n");
        return -1;
    }

    con_clear();

    _kernel_outStringFormat("0x%08x %d\n", (unsigned) m_addr, (int) m_len);
    hex_monitor((void *) m_addr, m_len);

    return 0;
}