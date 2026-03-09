
***

* Full name: `\CoralMedia\IR\Text`
* This class is marked as **final** and can't be subclassed
* This class is a **Final class**

## Methods

### tokenize

Tokenizes an array of texts, optionally applying normalization and stemming.

```php
public static tokenize(array $texts, bool $lowercase = true, bool $stripDiacritics = false, bool $stem = false, string $stemLanguage = "english", string $pattern = '/\s+/u', array $stopwords = []): array<int,array<int,string>>
```

* This method is **static**.
**Parameters:**

| Parameter          | Type       | Description                                                                                                    |
|--------------------|------------|----------------------------------------------------------------------------------------------------------------|
| `$texts`           | **array**  | List of input texts.                                                                                           |
| `$lowercase`       | **bool**   | Whether to lowercase terms.                                                                                    |
| `$stripDiacritics` | **bool**   | Whether to remove accented characters.                                                                         |
| `$stem`            | **bool**   | Whether to apply Snowball stemming.                                                                            |
| `$stemLanguage`    | **string** | Snowball language (for example "english", "spanish").                                                          |
| `$pattern`         | **string** | Regex delimiter pattern used for tokenization.                                                                 |
| `$stopwords`       | **array**  | Stopword list; it is normalized/stemmed with the same options.
Snowball encoding is internally fixed to UTF_8. |

***

### stem

Stems a single word using Snowball.

```php
public static stem(string $word, string $language = "english"): string
```

* This method is **static**.
**Parameters:**

| Parameter   | Type       | Description                                                                                                |
|-------------|------------|------------------------------------------------------------------------------------------------------------|
| `$word`     | **string** | Input term to stem.                                                                                        |
| `$language` | **string** | Snowball language name (for example "english", "spanish").
Snowball encoding is internally fixed to UTF_8. |

***
