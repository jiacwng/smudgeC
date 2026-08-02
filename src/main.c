#include <stdio.h>

#include "scanner.h"
#include "path_utils.h"
#include "cli.h"


int main(int argc, char **argv)
{

    /* FILE MANAGEMENT ---------------------------------------------------- */
    ScannerOptions options;
    scanner_options_init(&options);
    char *input_path;

    // argument parsing
    
    int parse_result = parse_arguments(argc, argv, &options, &input_path);

    if (parse_result == CLI_HELP)
    {
        return 0;
    }

    if (parse_result != CLI_OK)
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


    if (scan_file(input_file, output_file, options) != SCANNER_OK)
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

