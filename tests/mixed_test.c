#include <stdlib.h>

int main(void)
{
    int *a = malloc(100 * sizeof(int));
    int *b = malloc(200 * sizeof(int));
    int *c = malloc(50 * sizeof(int));

    if (a == NULL || b == NULL || c == NULL) {
        return 1;
    }

    free(a);

    /* b and c are intentionally not freed */

    return 0;
}
