#include <stdio.h>

typedef struct
{
    int value;
} Box;

int main(void)
{
    Box box;
    box.value = 3;

    switch (box.value)
    {
        case 3:
            printf("%lu\n", (unsigned long)sizeof(box));
            break;
        default:
            printf("default\n");
            break;
    }

    return 0;
}