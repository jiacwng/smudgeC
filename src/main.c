#include <stdio.h>

#include "scanner.h"
#include "names.h"
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


    char input_dir[256];
    get_input_directory(input_path, input_dir, sizeof(input_dir));

    NameSet protected_names;
    name_set_init(&protected_names);
    if (load_protected_names("data/protected_names.txt", &protected_names) != 1)
    {
        printf("smudgeC: could not read data/protected_names.txt\n");
        name_set_free(&protected_names);
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    if (scan_file(input_file, output_file, options, &protected_names, input_dir) != SCANNER_OK)
    {
        printf("smudgeC: failed to scan input\n");
        name_set_free(&protected_names);
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    printf("wrote: %s\n", output_path);

    name_set_free(&protected_names);
    fclose(input_file);
    fclose(output_file);
    return 0;
}

