# SmudgeC

SmudgeC is a source-to-source obfuscator for C, written in C. It reads one `.c`
file, transforms it, and writes the result to `out/<name>_obfuscated.c`. The
output is still C and compiles with `gcc` or `clang`.

## What it does

- Renames user identifiers with a symbol table, using a configurable prefix.
- Keeps C keywords, standard-library and POSIX names, macros, and names declared
  in your own `"..."` headers unchanged, so the output still builds and links.
- Strips comments.
- Encodes integer literals with varied value-preserving templates.
- Encodes string bytes as hex escapes.
- Hides string arguments by decrypting them at runtime, so the plaintext is not
  in the compiled binary.
- Minifies whitespace.
- Writes a rename map to `out/<name>.map`.
- Compiles the output and refuses it if it does not build.

## Build

```sh
make
```

On Windows with MinGW:

```sh
mingw32-make
```

## Usage

```sh
smudgec [options] input.c
```

Examples:

```sh
smudgec examples/hello.c
smudgec -a examples/hello.c                        # all safe passes
smudgec -a --no-strip-comments examples/hello.c    # all safe passes except one
smudgec --hide-strings --minify examples/hello.c
smudgec --prefix _x examples/hello.c
```

## Options

```text
-a, --all               enable all safe passes (rename, comments, ints, strings)
    --strip-comments    strip comments
    --encode-strings    encode string literal bytes as hex
    --hide-strings      decrypt string arguments at runtime
    --encode-ints       encode integer literals
    --minify            collapse whitespace and line breaks
    --prefix NAME       set the rename prefix (default _sm)
    --no-verify         skip the compile check of the output
    --no-<pass>         disable one pass of -a, e.g. -a --no-strip-comments
-h, --help              show help
```

## How it works

SmudgeC scans the source character by character with a lexical scanner. It has
no full C parser, so it decides what to rename from lists: keywords, a protected
name file (`data/protected_names.txt`), macros it finds in the file, and
identifiers from your local headers. After writing the output it compiles it with
the system compiler and refuses to present code that does not build.

## Limits

- One `.c` file at a time.
- The scanner is lexical, not a parser. It cannot always tell a declaration from
  a use, so it stays conservative and relies on the protected lists and the
  compile check.
- Struct fields and symbols reached only through indirect headers can be renamed
  wrongly. The compile check catches these and refuses the output.
- String hiding only transforms strings in function-argument position. Other
  strings stay as they are.
- Integer and hex string encoding change the source text only. The optimizer
  folds them away, so the compiled binary is unchanged. String hiding is the pass
  that protects the binary.
- Full correctness for the harder cases needs a real parser. That is the next
  direction.

## Tests

```sh
make test        # end-to-end: obfuscate fixtures, compile, compare output
make unit        # unit tests (Unity)
```
