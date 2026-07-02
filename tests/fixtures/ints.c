#include <stdio.h>

int add_bonus(int value)
{
    int bonus = 7;
    return value + bonus + 42;
}

int main(void)
{
    int start = 10;
    int result = add_bonus(start);

    printf("%d\n", result);
    return 0;
}