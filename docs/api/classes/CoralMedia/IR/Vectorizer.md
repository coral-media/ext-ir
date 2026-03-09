
***

* Full name: `\CoralMedia\IR\Vectorizer`
* This class is marked as **final** and can't be subclassed
* This class is a **Final class**

## Methods

### frequency

Computes term frequency per item from tokenized input.

```php
public static frequency(array $tokenizedItems): array<int,array<string,int>>
```

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                   |
|-------------------|-----------|-------------------------------|
| `$tokenizedItems` | **array** | Output from Text::tokenize(). |

**Return Value:**

Sparse TF maps (no zero-count terms).

***

### vocabulary

Computes document frequency (DF) from tokenized input.

```php
public static vocabulary(array $tokenizedItems): array<string,int>
```

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                   |
|-------------------|-----------|-------------------------------|
| `$tokenizedItems` | **array** | Output from Text::tokenize(). |

***

### fit

Fits a TF-IDF model from tokenized input.

```php
public static fit(array $tokenizedItems): array{vocabulary: array<string,int>, df: array<string,int>, idf: array<string,float>, documents: int}
```

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                   |
|-------------------|-----------|-------------------------------|
| `$tokenizedItems` | **array** | Output from Text::tokenize(). |

***

### transform

Transforms tokenized input into TF-IDF using a fitted model.

```php
public static transform(array $tokenizedItems, array $model): array<int,array<string,float>>
```

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                   |
|-------------------|-----------|-------------------------------|
| `$tokenizedItems` | **array** | Output from Text::tokenize(). |
| `$model`          | **array** | Model returned by fit().      |

**Return Value:**

Sparse TF-IDF maps per item.

***

### fitTransform

Fits a TF-IDF model and transforms the same tokenized input.

```php
public static fitTransform(array $tokenizedItems): array{model: array, matrix: array}
```

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                   |
|-------------------|-----------|-------------------------------|
| `$tokenizedItems` | **array** | Output from Text::tokenize(). |

***

### tfIdf

Computes TF-IDF weights from tokenized input.

```php
public static tfIdf(array $tokenizedItems): array<int,array<string,float>>
```

Uses smoothed IDF: log((N + 1) / (df + 1)) + 1.

* This method is **static**.
**Parameters:**

| Parameter         | Type      | Description                   |
|-------------------|-----------|-------------------------------|
| `$tokenizedItems` | **array** | Output from Text::tokenize(). |

**Return Value:**

Sparse TF-IDF maps per item.

***
