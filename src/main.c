#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "path_utils.h"


int main(int argc, char **argv)
{

    /* FILE MANAGEMENT ---------------------------------------------------- */
    ScannerOptions options;
    options.strip_comments = 0;
    options.encode_ints = 0;
    char *input_path;

    // argument parsing

    input_path = NULL;

    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if(strcmp(arg,"--strip-comments") == 0)
        {
            options.strip_comments = 1;
        }
        else if (strcmp(arg, "--encode-ints") == 0)
        {
            options.encode_ints = 1;
        }
        else if (strcmp(arg, "--help") == 0)
        {
            printf("usage: smudgec [options] input.c\n");
            printf("  -s, --strip-comments    strip comments\n");
            printf("  -e, --encode-ints       encode decimal integer literals\n");
            printf("  -h, --help              show help\n");
            return 0;
        }
        /* unknown long option */
        else if (arg[0] == '-' && arg[1] == '-')
        {
            printf("unknown option: %s\n", arg);
            printf("usage: smudgec [-s] [-e] input.c\n");
            return 1;
        }
        else if (arg[0] == '-' && arg[1] != '\0')
        {
            for (int j = 1; arg[j] != '\0'; j++)
            {
                if (arg[j] == 's')
                {
                    options.strip_comments = 1;
                }
                else if (arg[j] == 'e')
                {
                    options.encode_ints = 1;
                }
                else if (arg[j] == 'h')
                {
                    printf("usage: smudgec [options] input.c\n");
                    printf("  -s, --strip-comments    strip comments\n");
                    printf("  -e, --encode-ints       encode decimal integer literals\n");
                    printf("  -h, --help              show help\n");
                    return 0;
                }
                else
                {
                    printf("unknown option: -%c\n", arg[j]);
                    printf("usage: smudgec [-s] [-e] input.c\n");
                    return 1;
                }
            }
        }
        else
        {
            if (input_path != NULL)
            {
                printf("only one input file is supported\n");
                return 1;
            }

            input_path = arg;
        }
    }

    if (input_path == NULL)
    {
        printf("usage: smudgec [-s] [-e] input.c\n");
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

