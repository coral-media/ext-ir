<?php

/**
 * @generate-class-entries
 */

namespace {
    /** Returns the extension version. */
    function ir_version(): string {}
}

namespace CoralMedia\IR {
    final class LinearAlgebra
    {
        /**
         * Computes the dot product between two vectors.
         *
         * @param array $x First vector.
         * @param array $y Second vector. Must have same length as `$x`.
         */
        public static function dot(array $x, array $y): float {}

        /**
         * Computes the Euclidean norm (L2) of a vector.
         *
         * @param array $x Input vector.
         */
        public static function normL2(array $x): float {}

    }

    final class Text
    {
        /**
         * Tokenizes an array of texts, optionally applying normalization and stemming.
         *
         * @param array $texts List of input texts.
         * @param bool $lowercase Whether to lowercase terms.
         * @param bool $stripDiacritics Whether to remove accented characters.
         * @param bool $stem Whether to apply Snowball stemming.
         * @param string $stemLanguage Snowball language (for example "english", "spanish").
         * @param string $pattern Regex delimiter pattern used for tokenization.
         * @param array $stopwords Stopword list; it is normalized/stemmed with the same options.
         * Snowball encoding is internally fixed to UTF_8.
         * @return array<int, array<int, string>>
         */
        public static function tokenize(
            array $texts,
            bool $lowercase = true,
            bool $stripDiacritics = false,
            bool $stem = false,
            string $stemLanguage = "english",
            string $pattern = '/\s+/u',
            array $stopwords = []
        ): array {}

        /**
         * Stems a single word using Snowball.
         *
         * @param string $word Input term to stem.
         * @param string $language Snowball language name (for example "english", "spanish").
         * Snowball encoding is internally fixed to UTF_8.
         */
        public static function stem(string $word, string $language = "english"): string {}
    }

    final class Vectorizer
    {
        /**
         * Computes term frequency per item from tokenized input.
         *
         * @param array $tokenizedItems Output from Text::tokenize().
         * @return array<int, array<string, int>> Sparse TF maps (no zero-count terms).
         */
        public static function frequency(array $tokenizedItems): array {}

        /**
         * Computes document frequency (DF) from tokenized input.
         *
         * @param array $tokenizedItems Output from Text::tokenize().
         * @return array<string, int>
         */
        public static function vocabulary(array $tokenizedItems): array {}

        /**
         * Fits a TF-IDF model from tokenized input.
         *
         * @param array $tokenizedItems Output from Text::tokenize().
         * @return array{vocabulary: array<string, int>, df: array<string, int>, idf: array<string, float>, documents: int}
         */
        public static function fit(array $tokenizedItems): array {}

        /**
         * Transforms tokenized input into TF-IDF using a fitted model.
         *
         * @param array $tokenizedItems Output from Text::tokenize().
         * @param array $model Model returned by fit().
         * @return array<int, array<string, float>> Sparse TF-IDF maps per item.
         */
        public static function transform(array $tokenizedItems, array $model): array {}

        /**
         * Fits a TF-IDF model and transforms the same tokenized input.
         *
         * @param array $tokenizedItems Output from Text::tokenize().
         * @return array{model: array, matrix: array}
         */
        public static function fitTransform(array $tokenizedItems): array {}

        /**
         * Computes TF-IDF weights from tokenized input.
         * Uses smoothed IDF: log((N + 1) / (df + 1)) + 1.
         *
         * @param array $tokenizedItems Output from Text::tokenize().
         * @return array<int, array<string, float>> Sparse TF-IDF maps per item.
         */
        public static function tfIdf(array $tokenizedItems): array {}

        /**
         * Converts sparse vectors into dense vectors using the fitted vocabulary order.
         *
         * @param array $matrix Sparse vectors, typically returned by transform(), fitTransform(), or tfIdf().
         * @param array $model Model returned by fit() or fitTransform()["model"].
         * @return array<int, array<int, float>> Dense vectors aligned to the model vocabulary indexes.
         */
        public static function densify(array $matrix, array $model): array {}
    }

    final class Similarity
    {
        /**
         * Computes Pearson correlation between two vectors.
         *
         * @param array $x First vector as a dense positional list or sparse associative map.
         * @param array $y Second vector. Must match the shape of `$x`; dense lists must also have the same length.
         */
        public static function pearson(array $x, array $y): float {}

        /**
         * Computes cosine similarity between two vectors.
         *
         * @param array $x First vector as a dense positional list or sparse associative map.
         * @param array $y Second vector. Must match the shape of `$x`; dense lists must also have the same length.
         */
        public static function cosine(array $x, array $y): float {}

        /**
         * Computes Euclidean similarity between two vectors.
         * Similarity is defined as 1 / (1 + euclidean_distance).
         *
         * @param array $x First vector as a dense positional list or sparse associative map.
         * @param array $y Second vector. Must match the shape of `$x`; dense lists must also have the same length.
         */
        public static function euclidean(array $x, array $y): float {}

    }

    final class Ranking
    {
        /**
         * Returns the nearest item index for a query vector.
         *
         * @param array $query Sparse query vector (term => weight).
         * @param array $candidates List of sparse candidate vectors.
         * @param string $metric Similarity metric: "cosine", "euclidean", or "pearson".
         * @return int Index of the highest-scoring candidate, or -1 when `$candidates` is empty.
         */
        public static function nearest(array $query, array $candidates, string $metric = "cosine"): int {}

        /**
         * Returns top-K most similar items for a query vector.
         *
         * @param array $query Sparse query vector (term => weight).
         * @param array $candidates List of sparse candidate vectors.
         * @param int $k Number of results to return.
         * @param string $metric Similarity metric: "cosine", "euclidean", or "pearson".
         * @return array<int, array{index:int, score:float}>
         */
        public static function topK(array $query, array $candidates, int $k = 5, string $metric = "cosine"): array {}

        /**
         * Computes BM25 scores for a tokenized query against tokenized corpus items.
         * Uses Robertson/Sparck Jones IDF: log(1 + (N - df + 0.5) / (df + 0.5)).
         *
         * @param array $queryTokens Tokenized query terms.
         * @param array $tokenizedItems Tokenized corpus items, typically output from Text::tokenize().
         * @param float $k1 BM25 term saturation parameter.
         * @param float $b BM25 length normalization parameter.
         * @return array<int, float> Scores aligned with `$tokenizedItems`.
         */
        public static function bm25(array $queryTokens, array $tokenizedItems, float $k1 = 1.5, float $b = 0.75): array {}
    }
}
