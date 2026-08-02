#include <stdio.h>
#include <stdlib.h>

static int cmp(const void *a, const void *b)
{
    return (*(const int *)a - *(const int *)b);
}

int main(void)
{
    int values[3] = {3, 1, 2};
    qsort(values, 3, sizeof(int), cmp);
    printf("%d%d%d\n", values[0], values[1], values[2]);
    return 0;
}
