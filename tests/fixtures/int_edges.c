#include <stdio.h>

int main(void)
{
    unsigned int with_suffix = 42u;
    double decimal_value = 3.14;
    int normal_value = 8;

    printf("%u %.2f %d\n", with_suffix, decimal_value, normal_value);
    return 0;
}