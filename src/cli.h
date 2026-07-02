#ifndef CLI_H
#define CLI_H

#include "scanner.h"

#define CLI_OK 0
#define CLI_ERROR 1
#define CLI_HELP 2

int parse_arguments(
    int argc,
    char **argv,
    ScannerOptions *options,
    char **input_path
);

#endif