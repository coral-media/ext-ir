
***

* Full name: `\CoralMedia\IR\Similarity`
* This class is marked as **final** and can't be subclassed
* This class is a **Final class**

## Methods

### pearson

Computes Pearson correlation between two vectors.

```php
public static pearson(array $x, array $y): float
```

* This method is **static**.
**Parameters:**

| Parameter | Type      | Description                                                                              |
|-----------|-----------|------------------------------------------------------------------------------------------|
| `$x`      | **array** | First vector as a dense positional list or sparse associative map.                       |
| `$y`      | **array** | Second vector. Must match the shape of `$x`; dense lists must also have the same length. |

***

### cosine

Computes cosine similarity between two vectors.

```php
public static cosine(array $x, array $y): float
```

* This method is **static**.
**Parameters:**

| Parameter | Type      | Description                                                                              |
|-----------|-----------|------------------------------------------------------------------------------------------|
| `$x`      | **array** | First vector as a dense positional list or sparse associative map.                       |
| `$y`      | **array** | Second vector. Must match the shape of `$x`; dense lists must also have the same length. |

***

### euclidean

Computes Euclidean similarity between two vectors.

```php
public static euclidean(array $x, array $y): float
```

Similarity is defined as 1 / (1 + euclidean_distance).

* This method is **static**.
**Parameters:**

| Parameter | Type      | Description                                                                              |
|-----------|-----------|------------------------------------------------------------------------------------------|
| `$x`      | **array** | First vector as a dense positional list or sparse associative map.                       |
| `$y`      | **array** | Second vector. Must match the shape of `$x`; dense lists must also have the same length. |

***
