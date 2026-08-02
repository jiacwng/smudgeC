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
    int hide_strings;
    int minify;
    const char *prefix;
    int verify;
} ScannerOptions;

typedef struct
{
    FILE *input;
    FILE *output;
    ScannerOptions options;
    SymbolTable *table;
    NameSet *macros;
    NameSet *protected;
    const char *input_dir;
    FILE *map_file;
} ScanContext;


void scanner_options_init(ScannerOptions *options);

int scan_file(FILE *input_file, FILE *output_file, ScannerOptions options, NameSet *protected, const char *input_dir, FILE *map_file);

#endif