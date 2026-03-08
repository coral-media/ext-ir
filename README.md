# ext-ir

Information retrieval primitives for PHP.

Build requirements:

- System OpenBLAS is required.
- Linux/macOS: install OpenBLAS development package (for example `libopenblas-dev`).
- Windows: set `OPENBLAS_ROOT` to a prefix containing `include` and `lib\\openblas.lib`.
- Snowball is vendored under `lib/libstemmer` and built as part of the extension.

Current scaffold (`0.1.0`) includes:

- `ir_version()`
- `CoralMedia\IR\LinearAlgebra::dot(array $x, array $y): float`
- `CoralMedia\IR\Text::tokenize(string $text, bool $lowercase = true): array`

Next planned steps:

- vendor Snowball for stemming
- integrate BLAS-backed vector operations
- add end-to-end IR pipeline APIs
