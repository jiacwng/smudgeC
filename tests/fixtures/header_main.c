#include <stdio.h>
#include "header_lib.h"

int add_two(int value)
{
    return value + 2;
}

int main(void)
{
    int result = add_two(40);
    printf("%d\n", result);
    return 0;
}
