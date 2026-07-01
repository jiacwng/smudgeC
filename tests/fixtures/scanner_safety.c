#include <stdio.h>

int main(void)
{
    int visible_name = 7;

    // visible_name inside a line comment should stay readable
    /* visible_name inside a block comment should stay readable */

    printf("visible_name inside a string should stay readable\n");
    printf("%d\n", visible_name);

    return 0;
}