#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

typedef struct
{
    int strip_comments;
    int encode_ints;
} ScannerOptions;

int scan_file(FILE *input_file, FILE *output_file, ScannerOptions options);

#endif