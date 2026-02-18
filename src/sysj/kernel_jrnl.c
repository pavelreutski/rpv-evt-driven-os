#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "kernel_jrnl.h"
#include "kernel_stdio.h"

#define MAX_JOURNAL_SIZE            (2048)

static uint16_t j_rp                   = 0;
static uint16_t j_wp                   = 0;

static size_t j_len                    = MAX_JOURNAL_SIZE;

static char journal[MAX_JOURNAL_SIZE];

static void drop_oldestEntries(size_t entry_len);
static size_t j_read(char *entry, const size_t max_len);

bool _kernel_jnxtentry(char *entry, const size_t max_len, size_t *const entry_len) {

    if (entry_len == NULL) {
        return false;
    }

    size_t len = j_read(entry, max_len);
    *entry_len = len;

    return (len > 0);
}

void _kernel_jentry(char const* fmt, ...) {

    char entry[256];

    va_list args;
    va_start(args, fmt);
    
    _kernel_vstringFormat(entry, fmt, args);
    va_end(args);

    size_t entry_len = strlen(entry) + 1;

    if (entry_len > MAX_JOURNAL_SIZE) {
        return;
    }

    drop_oldestEntries(entry_len);

    size_t tail = (MAX_JOURNAL_SIZE - j_wp);

    if (tail >= entry_len) {

        memcpy(&journal[j_wp], entry, entry_len);
        j_wp += entry_len;

        if (j_wp == MAX_JOURNAL_SIZE) {
            j_wp = 0;
        }

    } else {

        size_t entry_tail = (entry_len - tail);

        memcpy(&journal[j_wp], entry, tail);
        memcpy(journal, entry + tail, entry_tail);

        j_wp = entry_tail;
    }

    j_len -= entry_len;
}

static void drop_oldestEntries(size_t entry_len) {

    while (j_len < entry_len) {        

        char c;
        size_t old_len = 0;

        do {

            c = journal[j_rp];

            old_len++;
            j_rp = (j_rp + 1) % MAX_JOURNAL_SIZE;

        } while(c != '\0');

        j_len += old_len;
    }
}

static size_t j_read(char *entry, const size_t max_len) {

    size_t r_len = 0;

    if (j_len == MAX_JOURNAL_SIZE) {
        return r_len;
    }

    char c;

    do {

        c = journal[j_rp];
        entry[r_len++] = c;

        j_len++;        
        j_rp = ((j_rp + 1) % MAX_JOURNAL_SIZE);

    } while (c != '\0' && r_len < max_len);

    if (r_len == max_len) {
        entry[r_len - 1] = '\0'; // terminate the string in case of max_len is reached
    }
    
    return r_len;
}
