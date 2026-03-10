
***

* Full name: `\CoralMedia\IR\Ranking`
* This class is marked as **final** and can't be subclassed
* This class is a **Final class**

## Methods

### nearest

Returns the nearest item index for a query vector.

```php
public static nearest(array $query, array $candidates, string $metric = "cosine"): int
```

* This method is **static**.
**Parameters:**

| Parameter     | Type       | Description                                             |
|---------------|------------|---------------------------------------------------------|
| `$query`      | **array**  | Sparse query vector (term => weight).                   |
| `$candidates` | **array**  | List of sparse candidate vectors.                       |
| `$metric`     | **string** | Similarity metric: "cosine", "euclidean", or "pearson". |

**Return Value:**

Index of the highest-scoring candidate, or -1 when `$candidates` is empty.

***

### topK

Returns top-K most similar items for a query vector.

```php
public static topK(array $query, array $candidates, int $k = 5, string $metric = "cosine"): array<int,array{index: int, score: float}>
```

* This method is **static**.
**Parameters:**

| Parameter     | Type       | Description                                             |
|---------------|------------|---------------------------------------------------------|
| `$query`      | **array**  | Sparse query vector (term => weight).                   |
| `$candidates` | **array**  | List of sparse candidate vectors.                       |
| `$k`          | **int**    | Number of results to return.                            |
| `$metric`     | **string** | Similarity metric: "cosine", "euclidean", or "pearson". |

***

### bm25

Computes BM25 scores for a tokenized query against tokenized corpus items.

```php
public static bm25(array $queryTokens, array $tokenizedItems, float $k1 = 1.5, float $b = 0.75): array<int,float>
```

Uses Robertson/Sparck Jones IDF: log(1 + (N - df + 0.5) / (df + 0.5)).

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                                                     |
|-------------------|-----------|-----------------------------------------------------------------|
| `$queryTokens`    | **array** | Tokenized query terms.                                          |
| `$tokenizedItems` | **array** | Tokenized corpus items, typically output from Text::tokenize(). |
| `$k1`             | **float** | BM25 term saturation parameter.                                 |
| `$b`              | **float** | BM25 length normalization parameter.                            |

**Return Value:**

Scores aligned with `$tokenizedItems`.

***
