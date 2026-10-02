#include <stdio.h>
#include "reporter.h"

void print_report(size_t total_allocations,
                  size_t total_frees,
                  size_t active_allocations,
                  size_t leaked_bytes)
{
    printf("\n========== LeakSpy Report ==========\n");

    printf("Total allocations : %zu\n", total_allocations);
    printf("Total frees       : %zu\n", total_frees);
    printf("Active allocations: %zu\n", active_allocations);
    printf("Leaked memory     : %zu bytes\n", leaked_bytes);

    printf("====================================\n");

    if (active_allocations > 0) {
        printf("Status: POSSIBLE MEMORY LEAK\n");
    } else {
        printf("Status: NO MEMORY LEAK DETECTED\n");
    }
}
