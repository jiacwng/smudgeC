#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "scanner.h"



int main(int argc, char **argv)
{

    /* FILE MANAGEMENT ---------------------------------------------------- */

    if(argc != 2)
    {
        printf("usage: smudgec input.c\n");
        return 1;
    }

    char *input_path = argv[1];
    char *slash = strrchr(input_path,'/');
    char *filename;
    // finding the filename from the folder
    if(slash != NULL)
    {
        filename = slash + 1;
    }
    else
    {  
        filename = input_path;
    }

    // finding the extension type from the filename
    char *dot = strrchr(filename,'.');
    if(dot == NULL)
    {
        printf("expected a .c file\n");
        return 1;
    }
    int name_length = dot - filename;
    char output_path[256];
    snprintf(output_path, sizeof(output_path), "out/%.*s_obfuscated.c", name_length, filename);


    FILE *input_file = fopen(input_path, "r");
    if(input_file == NULL)
    {
        perror("smudgec");
        return 1;
    }


    mkdir("out",0755);

    FILE *output_file = fopen(output_path, "w");
    if(output_file == NULL)
    {
        perror("smudgec");
        fclose(input_file);
        return 1;
    }

    /* ----------------------------------------------------------------- */

    if (scan_file(input_file,output_file) != 0)
    {
        printf("smudgeC: failed to scan input\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    printf("wrote: %s\n", output_path);


    fclose(input_file);
    fclose(output_file);
    return 0;
}

