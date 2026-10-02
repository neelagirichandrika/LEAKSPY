#include <stddef.h>
#include "leakspy.h"
#include "reporter.h"

#define MAX_ALLOCATIONS 1000

static Allocation allocations[MAX_ALLOCATIONS];
static size_t allocation_count = 0;

void track_allocation(void *address, size_t size)
{
    if (allocation_count >= MAX_ALLOCATIONS) {
        return;
    }

    allocations[allocation_count].address = address;
    allocations[allocation_count].size = size;
    allocations[allocation_count].active = 1;

    allocation_count++;
}

void track_deallocation(void *address)
{
    for (size_t i = 0; i < allocation_count; i++) {

        if (allocations[i].address == address &&
            allocations[i].active == 1) {

            allocations[i].active = 0;
            return;
        }
    }
}

void generate_report(void)
{
    size_t total_allocations = allocation_count;
    size_t total_frees = 0;
    size_t active_allocations = 0;
    size_t leaked_bytes = 0;

    for (size_t i = 0; i < allocation_count; i++) {

        if (allocations[i].active) {
            active_allocations++;
            leaked_bytes += allocations[i].size;
        } else {
            total_frees++;
        }
    }

    print_report(
        total_allocations,
        total_frees,
        active_allocations,
        leaked_bytes
    );
}
