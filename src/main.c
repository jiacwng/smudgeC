#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "names.h"
#include "path_utils.h"
#include "cli.h"


static int verify_output_compiles(const char *output_path, const char *input_dir)
{
    const char *cc = getenv("CC");
    if (cc == NULL || cc[0] == '\0')
    {
        cc = "cc";
    }

    char command[1024];
    int written;
    if (input_dir[0] != '\0')
    {
        written = snprintf(command, sizeof(command),
            "%s -fsyntax-only -I\"%s\" \"%s\"", cc, input_dir, output_path);
    }
    else
    {
        written = snprintf(command, sizeof(command),
            "%s -fsyntax-only \"%s\"", cc, output_path);
    }

    if (written < 0 || written >= (int)sizeof(command))
    {
        return 1;
    }

    return system(command);
}


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

    int names_loaded = 0;
    char exe_dir[256];
    if (get_executable_dir(exe_dir, sizeof(exe_dir)))
    {
        char data_path[512];
        int written = snprintf(data_path, sizeof(data_path),
            "%s/data/protected_names.txt", exe_dir);
        if (written > 0 && written < (int)sizeof(data_path))
        {
            names_loaded = load_protected_names(data_path, &protected_names);
        }
    }

    if (!names_loaded)
    {
        names_loaded = load_protected_names("data/protected_names.txt", &protected_names);
    }

    if (!names_loaded)
    {
        printf("smudgeC: could not read data/protected_names.txt\n");
        name_set_free(&protected_names);
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    char map_path[256];
    strcpy(map_path, output_path);
    char *map_suffix = strstr(map_path, "_obfuscated.c");
    FILE *map_file = NULL;
    if (map_suffix != NULL)
    {
        strcpy(map_suffix, ".map");
        map_file = fopen(map_path, "w");
    }

    if (scan_file(input_file, output_file, options, &protected_names, input_dir, map_file) != SCANNER_OK)
    {
        printf("smudgeC: failed to scan input\n");
        if (map_file != NULL)
        {
            fclose(map_file);
        }
        name_set_free(&protected_names);
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    printf("wrote: %s\n", output_path);

    if (map_file != NULL)
    {
        fclose(map_file);
    }
    name_set_free(&protected_names);
    fclose(input_file);
    fclose(output_file);

    if (options.verify && verify_output_compiles(output_path, input_dir) != 0)
    {
        printf("smudgeC: the obfuscated output did not pass a compile check\n");
        return 1;
    }

    return 0;
}

