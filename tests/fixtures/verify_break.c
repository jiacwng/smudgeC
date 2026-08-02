#include <stdio.h>
#include <time.h>

int main(void)
{
    struct tm value;
    value.tm_hour = 5;
    printf("%d\n", value.tm_hour);
    return 0;
}
