#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "path_utils.h"

int main(int argc, char **argv)
{

    /* FILE MANAGEMENT ---------------------------------------------------- */
    ScannerOptions options;
    options.strip_comments = 0;
    char *input_path;

    // argument parsing

    if (argc == 2)
    {
        input_path = argv[1];
    }
    else if (argc == 3 && strcmp(argv[1], "--strip-comments") == 0)
    {
        options.strip_comments = 1;
        input_path = argv[2];
    }
    else
    {
        printf("usage: smudgec [--strip-comments] input.c\n");
        return 1;
    }

    if (validate_input_path(input_path) != 0)
    {
        return 1;
    }

    // build output_path
    char output_path[256];
    if (build_output_path(input_path, output_path, sizeof(output_path)) != 0)
    {
        return 1;
    }



    FILE *input_file = fopen(input_path, "r");
    if(input_file == NULL)
    {
        perror("smudgec");
        return 1;
    }


    if (ensure_output_directory() != 0)
    {
        fclose(input_file);
        return 1;
    }

    FILE *output_file = fopen(output_path, "w");
    if(output_file == NULL)
    {
        perror("smudgec");
        fclose(input_file);
        return 1;
    }

    /* ----------------------------------------------------------------- */


    if (scan_file(input_file,output_file,options) != 0)
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

