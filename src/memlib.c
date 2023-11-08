#include "memlib.h"

#include <stdio.h>   // fprintf
#include <stdlib.h>  // malloc, free, exit
#include <errno.h>   // ENOMEM

#define MAX_HEAP (40*(1<<20))  /* 40 MB */

static char *mem_start_brk;
static char *mem_brk;
static char *mem_max_addr;

void mem_init(void) {
    mem_start_brk = malloc(MAX_HEAP);
    if (mem_start_brk == NULL) {
        fprintf(stderr, "Cannot allocate heap region\n");
        exit(1);
    }

    mem_max_addr = mem_start_brk + MAX_HEAP;
    mem_brk = mem_start_brk;
}

void mem_deinit(void) {
    free(mem_start_brk);
}

void mem_reset_brk() {
    mem_brk = mem_start_brk;
}

char *mem_sbrk(int incr) {
    char *old_brk = mem_brk;
    if (incr < 0 || (mem_brk + incr) > mem_max_addr) {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        return (void *)-1;
    }

    mem_brk += incr;
    return old_brk;
}

char *mem_heap_lo() {
    return mem_start_brk;  // first heap byte
}

char *mem_heap_hi() {
    return mem_brk - 1;  // last heap byte
}

long mem_heapsize() {
    return mem_brk - mem_start_brk;
}
