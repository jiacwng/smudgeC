#include "path_utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>   /* _mkdir on Windows */
#endif




int validate_input_path(const char *input_path)
{
    struct stat input_info;

    if (stat(input_path, &input_info) != 0)
    {
        perror("smudgec");
        return 1;
    }

    if (S_ISDIR(input_info.st_mode))
    {
        printf("input path is a directory\n");
        return 1;
    }

    return 0;
}

int ensure_output_directory(void)
{
    struct stat output_info;

    if (stat("out", &output_info) == 0)
    {
        if (S_ISDIR(output_info.st_mode))
        {
            return 0;
        }

        printf("out exists and is not a directory\n");
        return 1;
    }

    if (errno != ENOENT)
    {
        perror("smudgec");
        return 1;
    }

    #ifdef _WIN32
        if (_mkdir("out") != 0)
    #else
        if (mkdir("out", 0755) != 0)
    #endif
        {
            perror("smudgec");
            return 1;
        }

    return 0;

}

int build_output_path(const char *input_path, char *output_path, int output_path_size)
{
    const char *forward_slash = strrchr(input_path, '/');
    const char *backslash = strrchr(input_path, '\\');
    const char *separator = forward_slash;

    if (backslash != NULL && (separator == NULL || backslash > separator))
    {
        separator = backslash;
    }

    const char *filename;

    if (separator != NULL)
    {
        filename = separator + 1;
    }
    else
    {
        filename = input_path;
    }



    char *dot = strrchr(filename, '.');
    if (dot == NULL || strcmp(dot, ".c") != 0)
    {
        printf("expected a .c file\n");
        return 1;
    }

    int name_length = dot - filename;

    int written = snprintf(
        output_path,
        output_path_size,
        "out/%.*s_obfuscated.c",
        name_length,
        filename
    );

    if (written < 0 || written >= output_path_size)
    {
        printf("output path too long\n");
        return 1;
    }

    return 0;
}
