#ifndef LEAKSPY_H
#define LEAKSPY_H

#include <stddef.h>

typedef struct Allocation {
    void *address;
    size_t size;
    int active;
} Allocation;

void track_allocation(void *address, size_t size);
void track_deallocation(void *address);

void generate_report(void);

#endif
