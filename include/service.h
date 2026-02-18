#pragma once

struct service_s {

    const char *s_name;
    void (*const svc_main)(void);
};

typedef struct service_s service_t;

#define _SERVICE(name, service_main) \
    __attribute__((used, section(".service_table"))) \
    static const service_t name = { #name, service_main }
