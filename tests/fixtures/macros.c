#include <stdio.h>

#define WIDTH 80
#define SQUARE(x) ((x) * (x))
#define ADD(a, b) \
    ((a) + (b))

int main(void)
{
    int size = WIDTH;
    int area = SQUARE(size);
    int total = ADD(size, area);
    printf("%d %d %d\n", size, area, total);
    return 0;
}
