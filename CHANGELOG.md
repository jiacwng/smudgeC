# Changelog

## v1.1.0

### Added
- Runtime string decryption that keeps plaintext out of the binary (`--hide-strings`).
- Varied integer encoding with a seeded PRNG (four value-preserving templates).
- Whitespace minification (`--minify`).
- Configurable rename prefix (`--prefix`).
- `--no-<pass>` to disable one pass of `-a`.
- Rename map written to `out/<name>.map`.
- Compile check that refuses output which does not build.
- Macro name preservation, including multi-line macros.
- Local-header symbol preservation.
- Unit tests (Unity), hardened CI, CodeQL, and Dependabot.

### Changed
- Protected names moved from hardcoded C into an editable data file, now with POSIX names.
- Scanner state bundled into a single context.
- Data file resolved relative to the executable, so the tool runs from any directory.

### Fixed
- Portable directory creation on Windows and POSIX.

## v1.0.0

### Added
- Lexical scanner and identifier renaming.
- Comment stripping.
- Integer and string byte encoding.
