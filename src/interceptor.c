#define _GNU_SOURCE

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include "leakspy.h"

static void *(*real_malloc)(size_t) = NULL;
static void (*real_free)(void *) = NULL;

static int initializing = 0;

static void initialize_real_functions(void)
{
    if (real_malloc != NULL && real_free != NULL) {
        return;
    }

    if (initializing) {
        return;
    }

    initializing = 1;

    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free = dlsym(RTLD_NEXT, "free");

    initializing = 0;
}

void *malloc(size_t size)
{
    initialize_real_functions();

    if (real_malloc == NULL) {
        return NULL;
    }

    void *ptr = real_malloc(size);

    if (ptr != NULL && !initializing) {
        track_allocation(ptr, size);
    }

    return ptr;
}

void free(void *ptr)
{
    initialize_real_functions();

    if (ptr != NULL) {
        track_deallocation(ptr);
    }

    if (real_free != NULL) {
        real_free(ptr);
    }
}

__attribute__((destructor))
static void leakspy_finish(void)
{
    generate_report();
}
