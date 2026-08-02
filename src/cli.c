#include "cli.h"

#include <stdio.h>
#include <string.h>

static void print_usage(void)
{
    printf("usage: smudgec [options] input.c\n");
    printf("  -a, --all               enable all obfuscation passes\n");
    printf("      --strip-comments    strip comments\n");
    printf("      --encode-strings    encode string literal bytes\n");
    printf("      --hide-strings      decrypt string arguments at runtime\n");
    printf("      --encode-ints       encode decimal integer literals\n");
    printf("      --no-verify         skip the compile check of the output\n");
    printf("      --minify            collapse whitespace and line breaks\n");
    printf("      --prefix NAME       set the rename prefix (default _sm)\n");
    printf("  disable one pass of -a with --no-<pass>, e.g. -a --no-strip-comments\n");
    printf("  -h, --help              show help\n");
}

int parse_arguments(
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
            return CLI_HELP;
        }
        else if (strcmp(arg, "--encode-strings") == 0)
        {
            options->encode_strings = 1;
        }
        else if (strcmp(arg, "--hide-strings") == 0)
        {
            options->hide_strings = 1;
        }
        else if (strcmp(arg, "--minify") == 0)
        {
            options->minify = 1;
        }
        else if (strcmp(arg, "--prefix") == 0)
        {
            i++;
            if (i >= argc)
            {
                printf("--prefix needs a value\n");
                return CLI_ERROR;
            }
            options->prefix = argv[i];
        }
        else if (strcmp(arg, "--all") == 0)
        {
            options->strip_comments = 1;
            options->encode_ints = 1;
            options->encode_strings = 1;
        }
        else if (strcmp(arg, "--no-verify") == 0)
        {
            options->verify = 0;
        }
        else if (strcmp(arg, "--no-strip-comments") == 0)
        {
            options->strip_comments = 0;
        }
        else if (strcmp(arg, "--no-encode-ints") == 0)
        {
            options->encode_ints = 0;
        }
        else if (strcmp(arg, "--no-encode-strings") == 0)
        {
            options->encode_strings = 0;
        }
        /* unknown long option */
        else if (arg[0] == '-' && arg[1] == '-')
        {
            printf("unknown option: %s\n", arg);
            print_usage();
            return CLI_ERROR;
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
                    return CLI_HELP;
                }
                else
                {
                    printf("unknown option: -%c\n", arg[j]);
                    print_usage();
                    return CLI_ERROR;
                }
            }
        }
        else
        {
            if (*input_path != NULL)
            {
                printf("only one input file is supported\n");
                return CLI_ERROR;
            }

            *input_path = arg;
        }
    }

    if (*input_path == NULL)
    {
        print_usage();
        return CLI_ERROR;
    }

    return CLI_OK;
}