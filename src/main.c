#include <stdio.h>
#include <string.h>

#include "scanner.h"
#include "path_utils.h"

static void print_usage(void)
{
    printf("usage: smudgec [options] input.c\n");
    printf("  -a, --all               enable all obfuscation passes\n");
    printf("      --strip-comments    strip comments\n");
    printf("      --encode-strings    encode string literal bytes\n");
    printf("      --encode-ints       encode decimal integer literals\n");
    printf("  -h, --help              show help\n");
}


static int parse_arguments(
    int argc,
    char **argv,
    ScannerOptions *options,
    char **input_path
)
{
    *input_path = NULL;

    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if(strcmp(arg,"--strip-comments") == 0)
        {
            options->strip_comments = 1;
        }
        else if (strcmp(arg, "--encode-ints") == 0)
        {
            options->encode_ints = 1;
        }
        else if (strcmp(arg, "--help") == 0)
        {
            print_usage();
            return 2;
        }
        else if (strcmp(arg, "--encode-strings") == 0)
        {
            options->encode_strings = 1;
        }
        else if (strcmp(arg, "--all") == 0)
        {
            options->strip_comments = 1;
            options->encode_ints = 1;
            options->encode_strings = 1;
        }
        /* unknown long option */
        else if (arg[0] == '-' && arg[1] == '-')
        {
            printf("unknown option: %s\n", arg);
            print_usage();
            return 1;
        }
        else if (arg[0] == '-' && arg[1] != '\0')
        {
            for (int j = 1; arg[j] != '\0'; j++)
            {
                if (arg[j] == 'a')
                {
                    options->strip_comments = 1;
                    options->encode_ints = 1;
                    options->encode_strings = 1;
                }
                else if (arg[j] == 'h')
                {
                    print_usage();
                    return 2;
                }
                else
                {
                    printf("unknown option: -%c\n", arg[j]);
                    print_usage();
                    return 1;
                }
            }
        }
        else
        {
            if (*input_path != NULL)
            {
                printf("only one input file is supported\n");
                return 1;
            }

            *input_path = arg;
        }
    }

    if (*input_path == NULL)
    {
        print_usage();
        return 1;
    }

    return 0;
}



int main(int argc, char **argv)
{

    /* FILE MANAGEMENT ---------------------------------------------------- */
    ScannerOptions options;
    options.strip_comments = 0;
    options.encode_ints = 0;
    options.encode_strings = 0;
    char *input_path;

    // argument parsing
    
    int parse_result = parse_arguments(argc, argv, &options, &input_path);

    if (parse_result == 2)
    {
        return 0;
    }

    if (parse_result != 0)
    {
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

