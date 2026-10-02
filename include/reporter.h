#ifndef REPORTER_H
#define REPORTER_H

#include <stddef.h>

void print_report(size_t total_allocations,
                  size_t total_frees,
                  size_t active_allocations,
                  size_t leaked_bytes);

#endif
