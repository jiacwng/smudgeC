# SmudgeC

SmudgeC is a lightweight C source-to-source obfuscator, written in C, made as a personal project to better understand how basic code obfuscation works.

The tool takes one `.c` file, scans it character by character, then writes an obfuscated version to `out/<name>_obfuscated.c`.

## Features

- single-file C input
- output written automatically in `out/`
- identifier renaming with a symbol table
- C keywords and common standard-library names kept unchanged
- optional comment stripping
- optional integer literal encoding
- optional string literal byte encoding
- preprocessor lines and character literals preserved
- tests that compile original and obfuscated files, then compare their output

## Build

```sh
make
```

## Usage

```sh
./smudgec [options] input.c
```

Examples:

```sh
./smudgec examples/hello.c
./smudgec --strip-comments examples/hello.c
./smudgec --encode-ints examples/hello.c
./smudgec --encode-strings examples/hello.c
./smudgec --all examples/hello.c
```

Options:

```text
-a, --all               enable all obfuscation passes
    --strip-comments    strip comments
    --encode-strings    encode string literal bytes
    --encode-ints       encode decimal integer literals
-h, --help              show help
```

## Output

```sh
./smudgec --all examples/hello.c
```

creates:

```text
out/hello_obfuscated.c
```

The generated file is still C code, it can be compiled with `gcc` or `clang`.

## Tests

```sh
make test
```

The tests compile small C fixtures, run SmudgeC on them, compile the obfuscated files, then compare the program outputs.

## Limits

- the scanner is lexical, it does not parse the full C grammar
- only one `.c` file is handled at a time
- renamed identifiers are mapped globally inside the file
- project-specific external APIs are not detected automatically
- integer encoding skips suffixes and floating-point values
- string encoding keeps existing escape sequences unchanged
