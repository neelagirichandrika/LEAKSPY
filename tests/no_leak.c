#include <stdlib.h>

int main(void)
{
    int *ptr = malloc(100 * sizeof(int));

    if (ptr == NULL) {
        return 1;
    }

    free(ptr);

    return 0;
}
