#define _GNU_SOURCE

#include <dlfcn.h>
#include <stddef.h>

#include "leakspy.h"

static void *(*real_malloc)(size_t) = NULL;
static void *(*real_calloc)(size_t, size_t) = NULL;
static void *(*real_realloc)(void *, size_t) = NULL;
static void (*real_free)(void *) = NULL;

static int initializing = 0;

static void initialize_real_functions(void)
{
    if (real_malloc != NULL &&
        real_calloc != NULL &&
        real_realloc != NULL &&
        real_free != NULL) {
        return;
    }

    if (initializing) {
        return;
    }

    initializing = 1;

    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    real_realloc = dlsym(RTLD_NEXT, "realloc");
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

void *calloc(size_t nmemb, size_t size)
{
    initialize_real_functions();

    if (real_calloc == NULL) {
        return NULL;
    }

    void *ptr = real_calloc(nmemb, size);

    if (ptr != NULL && !initializing) {
        track_allocation(ptr, nmemb * size);
    }

    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    initialize_real_functions();

    if (real_realloc == NULL) {
        return NULL;
    }

    /* realloc(NULL, size) behaves like malloc(size). */
    if (ptr == NULL) {
        void *new_ptr = real_realloc(NULL, size);

        if (new_ptr != NULL && !initializing) {
            track_allocation(new_ptr, size);
        }

        return new_ptr;
    }

    /* realloc(ptr, 0) behaves like free(ptr) on this implementation. */
    if (size == 0) {
        void *new_ptr = real_realloc(ptr, size);

        if (!initializing) {
            track_deallocation(ptr);
        }

        return new_ptr;
    }

    void *new_ptr = real_realloc(ptr, size);

    /*
     * If realloc succeeds, the old pointer is no longer valid.
     * Track the new pointer as a new active allocation.
     */
    if (new_ptr != NULL && !initializing) {
        track_deallocation(ptr);
        track_allocation(new_ptr, size);
    }

    return new_ptr;
}

void free(void *ptr)
{
    initialize_real_functions();

    if (ptr != NULL && !initializing) {
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
