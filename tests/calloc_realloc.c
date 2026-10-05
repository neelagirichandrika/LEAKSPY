#include <stdlib.h>

int main(void)
{
    int *a = calloc(10, sizeof(int));

    if (a == NULL) {
        return 1;
    }

    int *b = malloc(5 * sizeof(int));

    if (b == NULL) {
        free(a);
        return 1;
    }

    b = realloc(b, 20 * sizeof(int));

    if (b == NULL) {
        free(a);
        return 1;
    }

    free(a);
    free(b);

    return 0;
}
