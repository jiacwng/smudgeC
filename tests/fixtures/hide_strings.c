#include <stdio.h>

char *visible = "SEEN_GLOBAL";

int main(void)
{
    printf("HIDDEN_ARG\n");
    printf("%s\n", visible);
    return 0;
}
