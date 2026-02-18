#include <stdint.h>

#include "command.h"

#include "kernel_stdio.h"
#include "kernel_service.h"

static int svc_m(const int argc, const char **argv);

_SHELL_COMMAND(svc, svc_m);

int svc_m(const int argc, const char **argv) {
    
    (void) argc;
    (void) argv;

    int svc_n = 0;

    void const* svc = NULL;
    char const* svc_name = NULL;

    while ((svc = _kernel_service(svc, &svc_name)) != NULL) {

        svc_n++;
        _kernel_outStringFormat("%s\n", svc_name);
    }

    if(svc_n == 0) {
        _kernel_outString("no services\n");
    }

    return 0;
}
