#include <stdio.h>


int add_numbers(int left, int right)
{
    int result = left + right;
    return result;
}


int main(void)
{
    // comment says fake_identifier
    /* block comment says another_fake_identifier */

    char letter = 'x';
    char newline = '\n';
    printf("%c %c\n", letter, newline);
    printf("hello from example\n");
    puts("using puts too");

    int total = add_numbers(19, 23);
    printf("%d\n", total);
    return 0;
}