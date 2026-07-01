#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *items = malloc(sizeof(int) * 2);

    if (items == NULL)
    {
        return 1;
    }

    items[0] = 19;
    items[1] = 23;

    printf("%d\n", items[0] + items[1]);

    free(items);
    return 0;
}