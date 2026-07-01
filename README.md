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
- Dynamic symbol table storage
- Dynamic identifier buffer
- Behavior regression tests with `make test`

## Current Limitations

- SmudgeC uses lexical scanning rather than a full C parser.
- The tool currently supports one `.c` file at a time.
- Identifier renaming is global within a file and does not yet model C scopes.
- Common C keywords and common standard-library identifiers are preserved, but external project-specific APIs are not detected automatically.
- Comments are currently preserved by default.

## Testing

The test target compiles original fixtures, runs SmudgeC, compiles the obfuscated output, and compares original vs obfuscated program output.

Run:

```sh
make test
```

## Build

```sh
make
```


## Usage

```sh
./smudgec examples/hello.c
```