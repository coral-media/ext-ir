# coral-media/ext-ir

Information retrieval primitives for PHP.

Build requirements:

- System OpenBLAS is required.
- Linux/macOS: install OpenBLAS development package (for example `libopenblas-dev`).
- Windows: set `OPENBLAS_ROOT` to a prefix containing `include` and `lib\\openblas.lib`.
- Snowball is vendored under `lib/libstemmer` and built as part of the extension.

Install with PIE:

```bash
# from repository root
cd ext-ir
pie install -j"$(nproc)"
# from packagist
pie install coral-media/ext-ir
```

If OpenBLAS is installed in a non-default location:

```bash
cd ext-ir
OPENBLAS_PREFIX=/custom/prefix pie install -j"$(nproc)"
```

Build from source (manual):

```bash
cd ext-ir
phpize
./configure --with-php-config="$(command -v php-config)"
make -j"$(nproc)"
make test
sudo make install
```

Enable extension:

```ini
extension=ir
```

Windows notes:

- Ensure OpenBLAS is installed and `OPENBLAS_ROOT` points to the prefix.
- Build with the same PHP toolchain version/arch/thread-safety as target PHP.
- Prebuilt libraries availables under Releases section.

Current scaffold (`0.1.0`) includes:

- `ir_version()`
- `CoralMedia\IR\LinearAlgebra::{dot,normL2}()`
- `CoralMedia\IR\Text::{tokenize,stem}()`
- `CoralMedia\IR\Vectorizer::{frequency,vocabulary,fit,transform,fitTransform,tfIdf,densify}()`
- `CoralMedia\IR\Similarity::{pearson,cosine,euclidean}()`
- `CoralMedia\IR\Ranking::{nearest,topK,bm25}()`

Usage example:

```php
<?php

$items = [
    "The quick brown fox jumps",
    "A quick fox is running",
    "Neural search with sparse vectors",
];

$tokenized = CoralMedia\IR\Text::tokenize(
    $items,
    pattern: '/\s+/u',
    lowercase: true,
    stripDiacritics: true,
    stem: true,
    language: 'english',
    stopwords: ['the', 'a', 'is', 'with']
);

$model = CoralMedia\IR\Vectorizer::fit($tokenized);
$matrix = CoralMedia\IR\Vectorizer::transform($tokenized, $model);

$query = $matrix[0];
$best = CoralMedia\IR\Ranking::nearest($query, $matrix, 'cosine');
$top2 = CoralMedia\IR\Ranking::topK($query, $matrix, 2, 'cosine');
```

Next planned steps:

- add ranking helpers (for example bm25)
- add packed-vector paths for dense workloads
- expand test coverage for edge cases and large inputs
