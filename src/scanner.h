#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

#include "names.h"

#define SCANNER_OK 0
#define SCANNER_ERROR 1

typedef struct
{
    int strip_comments;
    int encode_ints;
    int encode_strings;
    int verify;
} ScannerOptions;


void scanner_options_init(ScannerOptions *options);

int scan_file(FILE *input_file, FILE *output_file, ScannerOptions options, NameSet *protected, const char *input_dir, FILE *map_file);

#endif