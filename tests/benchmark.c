#include <stdlib.h>

#define ITERATIONS 100000

int main(void)
{
    for (int i = 0; i < ITERATIONS; i++) {
        int *ptr = malloc(100 * sizeof(int));

        if (ptr == NULL) {
            return 1;
        }

        free(ptr);
    }

    return 0;
}
