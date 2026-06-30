# SmudgeC

SmudgeC is a lightweight C source-to-source obfuscator written in C.

The project currently uses a custom lexical scanner to process C files character by character, preserve non-code regions such as strings, character literals, comments, and preprocessor directives, and rename user-defined identifiers through a symbol table while keeping keywords and protected standard-library names intact.

## Current Features

- CLI input for a single `.c` file
- Automatic output generation in `out/`
- Custom lexical scanner
- Identifier classification
- Symbol-table-based renaming
- Preservation of strings, character literals, comments, and preprocessor directives

## Build

```sh
make
```


## Usage

```sh
./smudgec examples/hello.c
```