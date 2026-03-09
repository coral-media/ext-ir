/* ir extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <math.h>
#include <string.h>

#include "php.h"
#include "ext/standard/info.h"
#include "php_ir.h"
#include "ir_arginfo.h"
#include <cblas.h>
#include <libstemmer.h>

static zend_class_entry *ir_ce_linear_algebra;
static zend_class_entry *ir_ce_text;
static zend_class_entry *ir_ce_vectorizer;
static zend_class_entry *ir_ce_similarity;

static inline double ir_cosine_from_dot_and_norms(double dot, double norm_a, double norm_b)
{
	double denom = norm_a * norm_b;
	return denom == 0.0 ? 0.0 : dot / denom;
}

static inline double ir_euclidean_similarity_from_dot_normsquared(double dot, double norm_a_squared, double norm_b_squared)
{
	double distance_squared = norm_a_squared + norm_b_squared - (2.0 * dot);
	if (distance_squared < 0.0) {
		distance_squared = 0.0;
	}

	return 1.0 / (1.0 + sqrt(distance_squared));
}

typedef struct {
	zend_long index;
	double score;
} ir_scored_index;

static bool ir_is_valid_utf8(const unsigned char *s, size_t len)
{
	size_t i = 0;
	while (i < len) {
		unsigned char c = s[i];
		if (c <= 0x7F) {
			i++;
			continue;
		}
		if ((c & 0xE0) == 0xC0) {
			if (i + 1 >= len || (s[i + 1] & 0xC0) != 0x80 || c < 0xC2) {
				return false;
			}
			i += 2;
			continue;
		}
		if ((c & 0xF0) == 0xE0) {
			if (i + 2 >= len || (s[i + 1] & 0xC0) != 0x80 || (s[i + 2] & 0xC0) != 0x80) {
				return false;
			}
			if (c == 0xE0 && s[i + 1] < 0xA0) {
				return false;
			}
			if (c == 0xED && s[i + 1] >= 0xA0) {
				return false;
			}
			i += 3;
			continue;
		}
		if ((c & 0xF8) == 0xF0) {
			if (i + 3 >= len || (s[i + 1] & 0xC0) != 0x80 || (s[i + 2] & 0xC0) != 0x80 || (s[i + 3] & 0xC0) != 0x80) {
				return false;
			}
			if (c == 0xF0 && s[i + 1] < 0x90) {
				return false;
			}
			if (c > 0xF4 || (c == 0xF4 && s[i + 1] > 0x8F)) {
				return false;
			}
			i += 4;
			continue;
		}
		return false;
	}
	return true;
}

static zend_string *ir_ensure_utf8(zend_string *input)
{
	const unsigned char *src = (const unsigned char *) ZSTR_VAL(input);
	size_t len = ZSTR_LEN(input);
	size_t i, j = 0;
	char *out;

	if (ir_is_valid_utf8(src, len)) {
		return zend_string_copy(input);
	}

	/* Fallback conversion assumes ISO-8859-1/latin1 bytes. */
	out = emalloc((len * 2) + 1);
	for (i = 0; i < len; i++) {
		unsigned char b = src[i];
		if (b < 0x80) {
			out[j++] = (char) b;
		} else {
			out[j++] = (char) (0xC0 | (b >> 6));
			out[j++] = (char) (0x80 | (b & 0x3F));
		}
	}
	out[j] = '\0';

	zend_string *result = zend_string_init(out, j, 0);
	efree(out);
	return result;
}

static zend_string *ir_strip_diacritics(zend_string *input)
{
	size_t i = 0, j = 0;
	const unsigned char *src = (const unsigned char *) ZSTR_VAL(input);
	size_t len = ZSTR_LEN(input);
	char *out = emalloc(len + 1);

	while (i < len) {
		unsigned char c = src[i];
		if (c == 0xC3 && i + 1 < len) {
			unsigned char d = src[i + 1];
			char mapped = 0;
			switch (d) {
				case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: mapped = 'A'; break;
				case 0x87: mapped = 'C'; break;
				case 0x88: case 0x89: case 0x8A: case 0x8B: mapped = 'E'; break;
				case 0x8C: case 0x8D: case 0x8E: case 0x8F: mapped = 'I'; break;
				case 0x91: mapped = 'N'; break;
				case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: mapped = 'O'; break;
				case 0x99: case 0x9A: case 0x9B: case 0x9C: mapped = 'U'; break;
				case 0x9D: case 0x9F: mapped = 'Y'; break;
				case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: mapped = 'a'; break;
				case 0xA7: mapped = 'c'; break;
				case 0xA8: case 0xA9: case 0xAA: case 0xAB: mapped = 'e'; break;
				case 0xAC: case 0xAD: case 0xAE: case 0xAF: mapped = 'i'; break;
				case 0xB1: mapped = 'n'; break;
				case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: mapped = 'o'; break;
				case 0xB9: case 0xBA: case 0xBB: case 0xBC: mapped = 'u'; break;
				case 0xBD: case 0xBF: mapped = 'y'; break;
				default: mapped = 0; break;
			}
			if (mapped != 0) {
				out[j++] = mapped;
			} else {
				out[j++] = (char) c;
				out[j++] = (char) d;
			}
			i += 2;
			continue;
		}
		out[j++] = (char) c;
		i++;
	}

	out[j] = '\0';
	zend_string *result = zend_string_init(out, j, 0);
	efree(out);
	return result;
}

static zend_string *ir_normalize_term(zend_string *input, bool lowercase, bool strip_diacritics)
{
	zend_string *result = zend_string_copy(input);
	if (strip_diacritics) {
		zend_string *tmp = ir_strip_diacritics(result);
		zend_string_release(result);
		result = tmp;
	}
	if (lowercase) {
		zend_string *tmp = zend_string_tolower(result);
		zend_string_release(result);
		result = tmp;
	}
	return result;
}

static zend_string *ir_stem_term(struct sb_stemmer *stemmer, zend_string *term)
{
	const sb_symbol *result;
	int result_len;

	result = sb_stemmer_stem(stemmer, (const sb_symbol *) ZSTR_VAL(term), (int) ZSTR_LEN(term));
	if (result == NULL) {
		return NULL;
	}

	result_len = sb_stemmer_length(stemmer);
	return zend_string_init((const char *) result, result_len, 0);
}

static bool ir_regex_split(zend_string *pattern, zend_string *subject, zval *out_tokens)
{
	zval fn, retval, params[4];

	ZVAL_STRING(&fn, "preg_split");
	ZVAL_STR_COPY(&params[0], pattern);
	ZVAL_STR_COPY(&params[1], subject);
	ZVAL_LONG(&params[2], -1);
	ZVAL_LONG(&params[3], 1);

	if (call_user_function(EG(function_table), NULL, &fn, &retval, 4, params) == FAILURE) {
		zval_ptr_dtor(&fn);
		zval_ptr_dtor(&params[0]);
		zval_ptr_dtor(&params[1]);
		return false;
	}

	zval_ptr_dtor(&fn);
	zval_ptr_dtor(&params[0]);
	zval_ptr_dtor(&params[1]);

	if (Z_TYPE(retval) != IS_ARRAY) {
		zval_ptr_dtor(&retval);
		return false;
	}

	ZVAL_COPY_VALUE(out_tokens, &retval);
	return true;
}

static zend_array *ir_build_stopset(
	zend_array *stopwords,
	bool lowercase,
	bool strip_diacritics,
	struct sb_stemmer *stemmer
)
{
	zval *val;
	zend_array *set = zend_new_array(zend_hash_num_elements(stopwords));

	ZEND_HASH_FOREACH_VAL(stopwords, val) {
		zend_string *raw = zval_get_string(val);
		zend_string *utf8 = ir_ensure_utf8(raw);
		zend_string *norm = ir_normalize_term(utf8, lowercase, strip_diacritics);
		zend_string_release(utf8);
		zend_string_release(raw);

		if (stemmer != NULL) {
			zend_string *stemmed = ir_stem_term(stemmer, norm);
			zend_string_release(norm);
			if (stemmed == NULL) {
				zend_array_destroy(set);
				return NULL;
			}
			norm = stemmed;
		}

		zend_hash_add_empty_element(set, norm);
		zend_string_release(norm);
	} ZEND_HASH_FOREACH_END();

	return set;
}

static double ir_sparse_dot(zend_array *a, zend_array *b)
{
	zval *val;
	zend_string *key;
	double sum = 0.0;
	zend_array *small = (zend_hash_num_elements(a) <= zend_hash_num_elements(b)) ? a : b;
	zend_array *large = (small == a) ? b : a;

	ZEND_HASH_FOREACH_STR_KEY_VAL(small, key, val) {
		zval *other;
		if (key == NULL) {
			continue;
		}
		other = zend_hash_find(large, key);
		if (other != NULL) {
			sum += zval_get_double(val) * zval_get_double(other);
		}
	} ZEND_HASH_FOREACH_END();

	return sum;
}

static double ir_sparse_norm2(zend_array *a)
{
	zval *val;
	double sum = 0.0;

	ZEND_HASH_FOREACH_VAL(a, val) {
		double x = zval_get_double(val);
		sum += x * x;
	} ZEND_HASH_FOREACH_END();

	return sqrt(sum);
}

static double ir_sparse_similarity(zend_array *query, zend_array *candidate, const char *metric)
{
	double dot = ir_sparse_dot(query, candidate);

	if (strcmp(metric, "cosine") == 0) {
		double nq = ir_sparse_norm2(query);
		double nc = ir_sparse_norm2(candidate);
		return ir_cosine_from_dot_and_norms(dot, nq, nc);
	}

	/* euclidean similarity = 1 / (1 + distance) */
	{
		double nq2 = ir_sparse_dot(query, query);
		double nc2 = ir_sparse_dot(candidate, candidate);
		return ir_euclidean_similarity_from_dot_normsquared(dot, nq2, nc2);
	}
}

static int ir_score_desc_cmp(const void *a, const void *b)
{
	const ir_scored_index *sa = (const ir_scored_index *) a;
	const ir_scored_index *sb = (const ir_scored_index *) b;
	if (sa->score < sb->score) return 1;
	if (sa->score > sb->score) return -1;
	if (sa->index > sb->index) return 1;
	if (sa->index < sb->index) return -1;
	return 0;
}

/* {{{ string ir_version() */
PHP_FUNCTION(ir_version)
{
	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_STRING(PHP_IR_VERSION);
}
/* }}} */

/* {{{ CoralMedia\IR\LinearAlgebra::dot() */
ZEND_METHOD(CoralMedia_IR_LinearAlgebra, dot)
{
	zval *x, *y;
	HashTable *hx, *hy;
	uint32_t n = 0, i = 0;
	double *x_values, *y_values;
	zval *vx, *vy;
	double result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY(x)
		Z_PARAM_ARRAY(y)
	ZEND_PARSE_PARAMETERS_END();

	hx = Z_ARRVAL_P(x);
	hy = Z_ARRVAL_P(y);
	n = zend_hash_num_elements(hx);

	if (n != zend_hash_num_elements(hy)) {
		zend_argument_value_error(2, "must have the same number of elements as argument #1 ($x)");
		RETURN_THROWS();
	}

	if (n == 0) {
		RETURN_DOUBLE(0.0);
	}

	x_values = (double *) safe_emalloc(n, sizeof(double), 0);
	y_values = (double *) safe_emalloc(n, sizeof(double), 0);

	ZEND_HASH_FOREACH_VAL(hx, vx) {
		x_values[i++] = zval_get_double(vx);
	} ZEND_HASH_FOREACH_END();

	i = 0;
	ZEND_HASH_FOREACH_VAL(hy, vy) {
		y_values[i++] = zval_get_double(vy);
	} ZEND_HASH_FOREACH_END();

	result = cblas_ddot((int) n, x_values, 1, y_values, 1);

	efree(x_values);
	efree(y_values);

	RETURN_DOUBLE(result);
}
/* }}} */

/* {{{ CoralMedia\IR\LinearAlgebra::normL2() */
ZEND_METHOD(CoralMedia_IR_LinearAlgebra, normL2)
{
	zval *x;
	HashTable *hx;
	uint32_t n = 0, i = 0;
	double *x_values;
	zval *vx;
	double result;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(x)
	ZEND_PARSE_PARAMETERS_END();

	hx = Z_ARRVAL_P(x);
	n = zend_hash_num_elements(hx);

	if (n == 0) {
		RETURN_DOUBLE(0.0);
	}

	x_values = (double *) safe_emalloc(n, sizeof(double), 0);

	ZEND_HASH_FOREACH_VAL(hx, vx) {
		x_values[i++] = zval_get_double(vx);
	} ZEND_HASH_FOREACH_END();

	result = cblas_dnrm2((int) n, x_values, 1);

	efree(x_values);

	RETURN_DOUBLE(result);
}
/* }}} */

/* {{{ CoralMedia\IR\Similarity::cosine() */
ZEND_METHOD(CoralMedia_IR_Similarity, cosine)
{
	zval *x, *y;
	HashTable *hx, *hy;
	uint32_t n = 0, i = 0;
	double *x_values, *y_values;
	zval *vx, *vy;
	double dot, nx, ny;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY(x)
		Z_PARAM_ARRAY(y)
	ZEND_PARSE_PARAMETERS_END();

	hx = Z_ARRVAL_P(x);
	hy = Z_ARRVAL_P(y);
	n = zend_hash_num_elements(hx);

	if (n != zend_hash_num_elements(hy)) {
		zend_argument_value_error(2, "must have the same number of elements as argument #1 ($x)");
		RETURN_THROWS();
	}

	if (n == 0) {
		RETURN_DOUBLE(0.0);
	}

	x_values = (double *) safe_emalloc(n, sizeof(double), 0);
	y_values = (double *) safe_emalloc(n, sizeof(double), 0);

	ZEND_HASH_FOREACH_VAL(hx, vx) {
		x_values[i++] = zval_get_double(vx);
	} ZEND_HASH_FOREACH_END();

	i = 0;
	ZEND_HASH_FOREACH_VAL(hy, vy) {
		y_values[i++] = zval_get_double(vy);
	} ZEND_HASH_FOREACH_END();

	dot = cblas_ddot((int) n, x_values, 1, y_values, 1);
	nx = cblas_dnrm2((int) n, x_values, 1);
	ny = cblas_dnrm2((int) n, y_values, 1);

	efree(x_values);
	efree(y_values);

	RETURN_DOUBLE(ir_cosine_from_dot_and_norms(dot, nx, ny));
}
/* }}} */

/* {{{ CoralMedia\IR\Similarity::euclidean() */
ZEND_METHOD(CoralMedia_IR_Similarity, euclidean)
{
	zval *x, *y;
	HashTable *hx, *hy;
	uint32_t n = 0, i = 0;
	double *x_values, *y_values;
	zval *vx, *vy;
	double dot, x_norm, y_norm;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY(x)
		Z_PARAM_ARRAY(y)
	ZEND_PARSE_PARAMETERS_END();

	hx = Z_ARRVAL_P(x);
	hy = Z_ARRVAL_P(y);
	n = zend_hash_num_elements(hx);

	if (n != zend_hash_num_elements(hy)) {
		zend_argument_value_error(2, "must have the same number of elements as argument #1 ($x)");
		RETURN_THROWS();
	}

	if (n == 0) {
		RETURN_DOUBLE(0.0);
	}

	x_values = (double *) safe_emalloc(n, sizeof(double), 0);
	y_values = (double *) safe_emalloc(n, sizeof(double), 0);

	ZEND_HASH_FOREACH_VAL(hx, vx) {
		x_values[i++] = zval_get_double(vx);
	} ZEND_HASH_FOREACH_END();

	i = 0;
	ZEND_HASH_FOREACH_VAL(hy, vy) {
		y_values[i++] = zval_get_double(vy);
	} ZEND_HASH_FOREACH_END();

	dot = cblas_ddot((int) n, x_values, 1, y_values, 1);
	x_norm = cblas_ddot((int) n, x_values, 1, x_values, 1);
	y_norm = cblas_ddot((int) n, y_values, 1, y_values, 1);

	efree(x_values);
	efree(y_values);

	RETURN_DOUBLE(ir_euclidean_similarity_from_dot_normsquared(dot, x_norm, y_norm));
}
/* }}} */

/* {{{ CoralMedia\IR\Text::stem() */
ZEND_METHOD(CoralMedia_IR_Text, stem)
{
	char *word, *language = "english";
	size_t word_len, language_len = sizeof("english") - 1;
	struct sb_stemmer *stemmer;
	zend_string *raw_word;
	zend_string *utf8_word;
	const sb_symbol *result;
	int result_len;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(word, word_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(language, language_len)
	ZEND_PARSE_PARAMETERS_END();

	stemmer = sb_stemmer_new(language, "UTF_8");
	if (stemmer == NULL) {
		zend_throw_error(NULL, "Failed to create Snowball stemmer for language '%s' with UTF_8 encoding", language);
		RETURN_THROWS();
	}

	raw_word = zend_string_init(word, word_len, 0);
	utf8_word = ir_ensure_utf8(raw_word);
	zend_string_release(raw_word);

	result = sb_stemmer_stem(stemmer, (const sb_symbol *) ZSTR_VAL(utf8_word), (int) ZSTR_LEN(utf8_word));
	if (result == NULL) {
		zend_string_release(utf8_word);
		sb_stemmer_delete(stemmer);
		zend_throw_error(NULL, "Snowball stemming failed");
		RETURN_THROWS();
	}

	result_len = sb_stemmer_length(stemmer);
	RETVAL_STRINGL((const char *) result, result_len);

	zend_string_release(utf8_word);
	sb_stemmer_delete(stemmer);
}
/* }}} */

/* {{{ CoralMedia\IR\Text::tokenize() */
ZEND_METHOD(CoralMedia_IR_Text, tokenize)
{
	zval *texts;
	zval *stopwords = NULL;
	bool lowercase = true;
	bool strip_diacritics = false;
	bool stem = false;
	char *stem_language = "english";
	size_t stem_language_len = sizeof("english") - 1;
	char *pattern_cstr = "/\\s+/u";
	size_t pattern_len = sizeof("/\\s+/u") - 1;
	struct sb_stemmer *stemmer = NULL;
	zend_array *stopset = NULL;
	zend_string *pattern;
	zval *doc_val;

	ZEND_PARSE_PARAMETERS_START(1, 7)
		Z_PARAM_ARRAY(texts)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(lowercase)
		Z_PARAM_BOOL(strip_diacritics)
		Z_PARAM_BOOL(stem)
		Z_PARAM_STRING(stem_language, stem_language_len)
		Z_PARAM_STRING(pattern_cstr, pattern_len)
		Z_PARAM_ARRAY(stopwords)
	ZEND_PARSE_PARAMETERS_END();

	if (stem) {
		stemmer = sb_stemmer_new(stem_language, "UTF_8");
		if (stemmer == NULL) {
			zend_throw_error(NULL, "Failed to create Snowball stemmer for language '%s' with UTF_8 encoding", stem_language);
			RETURN_THROWS();
		}
	}

	if (stopwords != NULL) {
		stopset = ir_build_stopset(Z_ARRVAL_P(stopwords), lowercase, strip_diacritics, stemmer);
		if (stopset == NULL) {
			if (stemmer != NULL) {
				sb_stemmer_delete(stemmer);
			}
			zend_throw_error(NULL, "Failed to preprocess stopwords");
			RETURN_THROWS();
		}
	} else {
		stopset = zend_new_array(0);
	}

	pattern = zend_string_init(pattern_cstr, pattern_len, 0);
	array_init(return_value);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(texts), doc_val) {
		zend_string *raw_doc = zval_get_string(doc_val);
		zend_string *utf8_doc = ir_ensure_utf8(raw_doc);
		zend_string *norm_doc = ir_normalize_term(utf8_doc, lowercase, strip_diacritics);
		zend_string_release(utf8_doc);
		zval split_tokens;
		zval out_doc;
		zval *token_val;

		zend_string_release(raw_doc);

		if (!ir_regex_split(pattern, norm_doc, &split_tokens)) {
			zend_string_release(norm_doc);
			zend_string_release(pattern);
			zend_array_destroy(stopset);
			if (stemmer != NULL) {
				sb_stemmer_delete(stemmer);
			}
			zend_throw_error(NULL, "preg_split failed while tokenizing documents");
			RETURN_THROWS();
		}

		zend_string_release(norm_doc);
		array_init(&out_doc);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(split_tokens), token_val) {
			zend_string *token = zval_get_string(token_val);

			if (stemmer != NULL) {
				zend_string *stemmed = ir_stem_term(stemmer, token);
				zend_string_release(token);
				if (stemmed == NULL) {
					zval_ptr_dtor(&split_tokens);
					zval_ptr_dtor(&out_doc);
					zend_string_release(pattern);
					zend_array_destroy(stopset);
					sb_stemmer_delete(stemmer);
					zend_throw_error(NULL, "Snowball stemming failed");
					RETURN_THROWS();
				}
				token = stemmed;
			}

			if (zend_hash_exists(stopset, token)) {
				zend_string_release(token);
				continue;
			}

			add_next_index_str(&out_doc, token);
		} ZEND_HASH_FOREACH_END();

		zval_ptr_dtor(&split_tokens);
		add_next_index_zval(return_value, &out_doc);
	} ZEND_HASH_FOREACH_END();

	zend_string_release(pattern);
	zend_array_destroy(stopset);
	if (stemmer != NULL) {
		sb_stemmer_delete(stemmer);
	}
}
/* }}} */

/* {{{ CoralMedia\IR\Vectorizer::frequency() */
ZEND_METHOD(CoralMedia_IR_Vectorizer, frequency)
{
	zval *tokenized_items;
	zval *doc_val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(tokenized_items)
	ZEND_PARSE_PARAMETERS_END();

	array_init(return_value);
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zval *token_val;
		zval out_doc;
		zend_array *doc_ht;

		if (Z_TYPE_P(doc_val) != IS_ARRAY) {
			zend_argument_value_error(1, "must be an array of token arrays");
			RETURN_THROWS();
		}

		array_init(&out_doc);
		doc_ht = Z_ARR(out_doc);

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *existing = zend_hash_find(doc_ht, token);

			if (existing == NULL) {
				zval one;
				ZVAL_LONG(&one, 1);
				zend_hash_add_new(doc_ht, token, &one);
			} else {
				Z_LVAL_P(existing)++;
			}

			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();

		add_next_index_zval(return_value, &out_doc);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ CoralMedia\IR\Vectorizer::vocabulary() */
ZEND_METHOD(CoralMedia_IR_Vectorizer, vocabulary)
{
	zval *tokenized_items;
	zend_array *term_to_index;
	zend_string **terms = NULL;
	uint32_t terms_count = 0, terms_cap = 0;
	zend_long *df = NULL;
	zval *doc_val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(tokenized_items)
	ZEND_PARSE_PARAMETERS_END();

	term_to_index = zend_new_array(64);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zend_array *seen;
		zval *token_val;
		if (Z_TYPE_P(doc_val) != IS_ARRAY) {
			zend_array_destroy(term_to_index);
			if (terms != NULL) {
				uint32_t i;
				for (i = 0; i < terms_count; i++) {
					zend_string_release(terms[i]);
				}
				efree(terms);
			}
			if (df != NULL) {
				efree(df);
			}
			zend_argument_value_error(1, "must be an array of token arrays");
			RETURN_THROWS();
		}

		seen = zend_new_array(16);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *pos = zend_hash_find(term_to_index, token);
			uint32_t index;

			if (pos == NULL) {
				zval new_pos;
				ZVAL_LONG(&new_pos, terms_count);
				zend_hash_add_new(term_to_index, token, &new_pos);
				if (terms_count == terms_cap) {
					uint32_t new_cap = terms_cap ? terms_cap * 2 : 16;
					terms = erealloc(terms, new_cap * sizeof(zend_string *));
					df = erealloc(df, new_cap * sizeof(zend_long));
					memset(df + terms_cap, 0, (new_cap - terms_cap) * sizeof(zend_long));
					terms_cap = new_cap;
				}
				terms[terms_count] = zend_string_copy(token);
				index = terms_count++;
			} else {
				index = (uint32_t) Z_LVAL_P(pos);
			}

			if (!zend_hash_exists(seen, token)) {
				zend_hash_add_empty_element(seen, token);
				df[index]++;
			}

			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();

		zend_array_destroy(seen);
	} ZEND_HASH_FOREACH_END();

	array_init(return_value);
	{
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			add_assoc_long_ex(return_value, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), df[i]);
		}
	}

	zend_array_destroy(term_to_index);
	if (terms != NULL) {
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			zend_string_release(terms[i]);
		}
		efree(terms);
	}
	if (df != NULL) {
		efree(df);
	}
}
/* }}} */

/* {{{ CoralMedia\IR\Vectorizer::tfIdf() */
ZEND_METHOD(CoralMedia_IR_Vectorizer, tfIdf)
{
	zval *tokenized_items;
	uint32_t doc_count;
	zend_array *term_to_index;
	zend_string **terms = NULL;
	zend_long *df = NULL;
	uint32_t terms_count = 0, terms_cap = 0;
	zval *doc_val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(tokenized_items)
	ZEND_PARSE_PARAMETERS_END();

	doc_count = zend_hash_num_elements(Z_ARRVAL_P(tokenized_items));
	term_to_index = zend_new_array(64);

	/* Pass 1: collect vocabulary and document frequencies. */
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zval *token_val;
		zend_array *seen;

		if (Z_TYPE_P(doc_val) != IS_ARRAY) {
			zend_array_destroy(term_to_index);
			if (terms != NULL) {
				uint32_t i;
				for (i = 0; i < terms_count; i++) {
					zend_string_release(terms[i]);
				}
				efree(terms);
			}
			if (df != NULL) {
				efree(df);
			}
			zend_argument_value_error(1, "must be an array of token arrays");
			RETURN_THROWS();
		}

		seen = zend_new_array(16);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *pos = zend_hash_find(term_to_index, token);
			uint32_t index;

			if (pos == NULL) {
				zval new_pos;
				ZVAL_LONG(&new_pos, terms_count);
				zend_hash_add_new(term_to_index, token, &new_pos);

				if (terms_count == terms_cap) {
					uint32_t new_cap = terms_cap ? terms_cap * 2 : 16;
					terms = erealloc(terms, new_cap * sizeof(zend_string *));
					df = erealloc(df, new_cap * sizeof(zend_long));
					memset(df + terms_cap, 0, (new_cap - terms_cap) * sizeof(zend_long));
					terms_cap = new_cap;
				}

				terms[terms_count] = zend_string_copy(token);
				index = terms_count++;
			} else {
				index = (uint32_t) Z_LVAL_P(pos);
			}

			if (!zend_hash_exists(seen, token)) {
				zend_hash_add_empty_element(seen, token);
				df[index]++;
			}

			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();
		zend_array_destroy(seen);
	} ZEND_HASH_FOREACH_END();

	array_init(return_value);

	/* Pass 2: compute sparse TF-IDF per item. */
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zval *token_val;
		zend_array *tf_map = zend_new_array(16);
		zval out_item;

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *existing = zend_hash_find(tf_map, token);
			if (existing == NULL) {
				zval one;
				ZVAL_LONG(&one, 1);
				zend_hash_add_new(tf_map, token, &one);
			} else {
				Z_LVAL_P(existing)++;
			}
			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();

		array_init(&out_item);
		{
			zend_string *term;
			zval *tf_val;
			ZEND_HASH_FOREACH_STR_KEY_VAL(tf_map, term, tf_val) {
				zval *pos = zend_hash_find(term_to_index, term);
				if (pos != NULL) {
					uint32_t idx = (uint32_t) Z_LVAL_P(pos);
					double idf = log(((double) doc_count + 1.0) / ((double) df[idx] + 1.0)) + 1.0;
					double weight = ((double) Z_LVAL_P(tf_val)) * idf;
					add_assoc_double_ex(&out_item, ZSTR_VAL(term), ZSTR_LEN(term), weight);
				}
			} ZEND_HASH_FOREACH_END();
		}

		zend_array_destroy(tf_map);
		add_next_index_zval(return_value, &out_item);
	} ZEND_HASH_FOREACH_END();

	zend_array_destroy(term_to_index);
	if (terms != NULL) {
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			zend_string_release(terms[i]);
		}
		efree(terms);
	}
	if (df != NULL) {
		efree(df);
	}
}
/* }}} */

/* {{{ CoralMedia\IR\Vectorizer::fit() */
ZEND_METHOD(CoralMedia_IR_Vectorizer, fit)
{
	zval *tokenized_items;
	uint32_t doc_count;
	zend_array *term_to_index;
	zend_string **terms = NULL;
	zend_long *df = NULL;
	uint32_t terms_count = 0, terms_cap = 0;
	zval *doc_val;
	zval vocab, df_map, idf_map;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(tokenized_items)
	ZEND_PARSE_PARAMETERS_END();

	doc_count = zend_hash_num_elements(Z_ARRVAL_P(tokenized_items));
	term_to_index = zend_new_array(64);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zval *token_val;
		zend_array *seen;

		if (Z_TYPE_P(doc_val) != IS_ARRAY) {
			zend_array_destroy(term_to_index);
			if (terms != NULL) {
				uint32_t i;
				for (i = 0; i < terms_count; i++) {
					zend_string_release(terms[i]);
				}
				efree(terms);
			}
			if (df != NULL) {
				efree(df);
			}
			zend_argument_value_error(1, "must be an array of token arrays");
			RETURN_THROWS();
		}

		seen = zend_new_array(16);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *pos = zend_hash_find(term_to_index, token);
			uint32_t index;

			if (pos == NULL) {
				zval new_pos;
				ZVAL_LONG(&new_pos, terms_count);
				zend_hash_add_new(term_to_index, token, &new_pos);

				if (terms_count == terms_cap) {
					uint32_t new_cap = terms_cap ? terms_cap * 2 : 16;
					terms = erealloc(terms, new_cap * sizeof(zend_string *));
					df = erealloc(df, new_cap * sizeof(zend_long));
					memset(df + terms_cap, 0, (new_cap - terms_cap) * sizeof(zend_long));
					terms_cap = new_cap;
				}

				terms[terms_count] = zend_string_copy(token);
				index = terms_count++;
			} else {
				index = (uint32_t) Z_LVAL_P(pos);
			}

			if (!zend_hash_exists(seen, token)) {
				zend_hash_add_empty_element(seen, token);
				df[index]++;
			}

			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();
		zend_array_destroy(seen);
	} ZEND_HASH_FOREACH_END();

	array_init(return_value);
	array_init(&vocab);
	array_init(&df_map);
	array_init(&idf_map);
	{
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			double idf = log(((double) doc_count + 1.0) / ((double) df[i] + 1.0)) + 1.0;
			add_assoc_long_ex(&vocab, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), i);
			add_assoc_long_ex(&df_map, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), df[i]);
			add_assoc_double_ex(&idf_map, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), idf);
		}
	}
	add_assoc_long(return_value, "documents", (zend_long) doc_count);
	add_assoc_zval(return_value, "vocabulary", &vocab);
	add_assoc_zval(return_value, "df", &df_map);
	add_assoc_zval(return_value, "idf", &idf_map);

	zend_array_destroy(term_to_index);
	if (terms != NULL) {
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			zend_string_release(terms[i]);
		}
		efree(terms);
	}
	if (df != NULL) {
		efree(df);
	}
}
/* }}} */

/* {{{ CoralMedia\IR\Vectorizer::transform() */
ZEND_METHOD(CoralMedia_IR_Vectorizer, transform)
{
	zval *tokenized_items;
	zval *model;
	zval *idf_zv;
	zend_array *idf_map;
	zval *doc_val;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY(tokenized_items)
		Z_PARAM_ARRAY(model)
	ZEND_PARSE_PARAMETERS_END();

	idf_zv = zend_hash_str_find(Z_ARRVAL_P(model), "idf", sizeof("idf") - 1);
	if (idf_zv == NULL || Z_TYPE_P(idf_zv) != IS_ARRAY) {
		zend_argument_value_error(2, "must contain an 'idf' array");
		RETURN_THROWS();
	}
	idf_map = Z_ARRVAL_P(idf_zv);

	array_init(return_value);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zval *token_val;
		zend_array *tf_map = zend_new_array(16);
		zval out_item;

		if (Z_TYPE_P(doc_val) != IS_ARRAY) {
			zend_array_destroy(tf_map);
			zend_argument_value_error(1, "must be an array of token arrays");
			RETURN_THROWS();
		}

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *existing = zend_hash_find(tf_map, token);
			if (existing == NULL) {
				zval one;
				ZVAL_LONG(&one, 1);
				zend_hash_add_new(tf_map, token, &one);
			} else {
				Z_LVAL_P(existing)++;
			}
			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();

		array_init(&out_item);
		{
			zend_string *term;
			zval *tf_val;
			ZEND_HASH_FOREACH_STR_KEY_VAL(tf_map, term, tf_val) {
				zval *idf_val = zend_hash_find(idf_map, term);
				if (idf_val != NULL) {
					double idf = zval_get_double(idf_val);
					double weight = ((double) Z_LVAL_P(tf_val)) * idf;
					add_assoc_double_ex(&out_item, ZSTR_VAL(term), ZSTR_LEN(term), weight);
				}
			} ZEND_HASH_FOREACH_END();
		}

		zend_array_destroy(tf_map);
		add_next_index_zval(return_value, &out_item);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ CoralMedia\IR\Vectorizer::fitTransform() */
ZEND_METHOD(CoralMedia_IR_Vectorizer, fitTransform)
{
	zval *tokenized_items;
	uint32_t doc_count;
	zend_array *term_to_index;
	zend_string **terms = NULL;
	zend_long *df = NULL;
	uint32_t terms_count = 0, terms_cap = 0;
	zval *doc_val;
	zval model;
	zval vocab, df_map, idf_map;
	zval matrix;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(tokenized_items)
	ZEND_PARSE_PARAMETERS_END();

	doc_count = zend_hash_num_elements(Z_ARRVAL_P(tokenized_items));
	term_to_index = zend_new_array(64);

	/* Build model terms/df. */
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
		zval *token_val;
		zend_array *seen;

		if (Z_TYPE_P(doc_val) != IS_ARRAY) {
			zend_array_destroy(term_to_index);
			if (terms != NULL) {
				uint32_t i;
				for (i = 0; i < terms_count; i++) {
					zend_string_release(terms[i]);
				}
				efree(terms);
			}
			if (df != NULL) {
				efree(df);
			}
			zend_argument_value_error(1, "must be an array of token arrays");
			RETURN_THROWS();
		}

		seen = zend_new_array(16);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
			zend_string *token = zval_get_string(token_val);
			zval *pos = zend_hash_find(term_to_index, token);
			uint32_t index;

			if (pos == NULL) {
				zval new_pos;
				ZVAL_LONG(&new_pos, terms_count);
				zend_hash_add_new(term_to_index, token, &new_pos);
				if (terms_count == terms_cap) {
					uint32_t new_cap = terms_cap ? terms_cap * 2 : 16;
					terms = erealloc(terms, new_cap * sizeof(zend_string *));
					df = erealloc(df, new_cap * sizeof(zend_long));
					memset(df + terms_cap, 0, (new_cap - terms_cap) * sizeof(zend_long));
					terms_cap = new_cap;
				}
				terms[terms_count] = zend_string_copy(token);
				index = terms_count++;
			} else {
				index = (uint32_t) Z_LVAL_P(pos);
			}

			if (!zend_hash_exists(seen, token)) {
				zend_hash_add_empty_element(seen, token);
				df[index]++;
			}

			zend_string_release(token);
		} ZEND_HASH_FOREACH_END();
		zend_array_destroy(seen);
	} ZEND_HASH_FOREACH_END();

	array_init(&model);
	array_init(&vocab);
	array_init(&df_map);
	array_init(&idf_map);
	{
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			double idf = log(((double) doc_count + 1.0) / ((double) df[i] + 1.0)) + 1.0;
			add_assoc_long_ex(&vocab, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), i);
			add_assoc_long_ex(&df_map, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), df[i]);
			add_assoc_double_ex(&idf_map, ZSTR_VAL(terms[i]), ZSTR_LEN(terms[i]), idf);
		}
	}
	add_assoc_long(&model, "documents", (zend_long) doc_count);
	add_assoc_zval(&model, "vocabulary", &vocab);
	add_assoc_zval(&model, "df", &df_map);
	add_assoc_zval(&model, "idf", &idf_map);

	/* Build matrix from idf map */
	array_init(&matrix);
	{
		zend_array *idf_ht = Z_ARR(idf_map);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(tokenized_items), doc_val) {
			zval *token_val;
			zend_array *tf_map = zend_new_array(16);
			zval out_item;

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(doc_val), token_val) {
				zend_string *token = zval_get_string(token_val);
				zval *existing = zend_hash_find(tf_map, token);
				if (existing == NULL) {
					zval one;
					ZVAL_LONG(&one, 1);
					zend_hash_add_new(tf_map, token, &one);
				} else {
					Z_LVAL_P(existing)++;
				}
				zend_string_release(token);
			} ZEND_HASH_FOREACH_END();

			array_init(&out_item);
			{
				zend_string *term;
				zval *tf_val;
				ZEND_HASH_FOREACH_STR_KEY_VAL(tf_map, term, tf_val) {
					zval *idf_val = zend_hash_find(idf_ht, term);
					if (idf_val != NULL) {
						double idf = zval_get_double(idf_val);
						double weight = ((double) Z_LVAL_P(tf_val)) * idf;
						add_assoc_double_ex(&out_item, ZSTR_VAL(term), ZSTR_LEN(term), weight);
					}
				} ZEND_HASH_FOREACH_END();
			}

			zend_array_destroy(tf_map);
			add_next_index_zval(&matrix, &out_item);
		} ZEND_HASH_FOREACH_END();
	}

	array_init(return_value);
	add_assoc_zval(return_value, "model", &model);
	add_assoc_zval(return_value, "matrix", &matrix);

	zend_array_destroy(term_to_index);
	if (terms != NULL) {
		uint32_t i;
		for (i = 0; i < terms_count; i++) {
			zend_string_release(terms[i]);
		}
		efree(terms);
	}
	if (df != NULL) {
		efree(df);
	}
}
/* }}} */

/* {{{ CoralMedia\IR\Similarity::nearest() */
ZEND_METHOD(CoralMedia_IR_Similarity, nearest)
{
	zval *query;
	zval *candidates;
	char *metric = "cosine";
	size_t metric_len = sizeof("cosine") - 1;
	zval *candidate;
	zend_long best_index = -1;
	zend_long idx = 0;
	double best_score = -INFINITY;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY(query)
		Z_PARAM_ARRAY(candidates)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(metric, metric_len)
	ZEND_PARSE_PARAMETERS_END();

	if (strcmp(metric, "cosine") != 0 && strcmp(metric, "euclidean") != 0) {
		zend_argument_value_error(3, "must be \"cosine\" or \"euclidean\"");
		RETURN_THROWS();
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(candidates), candidate) {
		double score;
		if (Z_TYPE_P(candidate) != IS_ARRAY) {
			zend_argument_value_error(2, "must be an array of sparse vectors");
			RETURN_THROWS();
		}
		score = ir_sparse_similarity(Z_ARRVAL_P(query), Z_ARRVAL_P(candidate), metric);
		if (best_index < 0 || score > best_score) {
			best_score = score;
			best_index = idx;
		}
		idx++;
	} ZEND_HASH_FOREACH_END();

	RETURN_LONG(best_index);
}
/* }}} */

/* {{{ CoralMedia\IR\Similarity::topK() */
ZEND_METHOD(CoralMedia_IR_Similarity, topK)
{
	zval *query;
	zval *candidates;
	zend_long k = 5;
	char *metric = "cosine";
	size_t metric_len = sizeof("cosine") - 1;
	uint32_t total, i = 0, out_n;
	ir_scored_index *scores;
	zval *candidate;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_ARRAY(query)
		Z_PARAM_ARRAY(candidates)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(k)
		Z_PARAM_STRING(metric, metric_len)
	ZEND_PARSE_PARAMETERS_END();

	if (strcmp(metric, "cosine") != 0 && strcmp(metric, "euclidean") != 0) {
		zend_argument_value_error(4, "must be \"cosine\" or \"euclidean\"");
		RETURN_THROWS();
	}
	if (k < 0) {
		zend_argument_value_error(3, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	total = zend_hash_num_elements(Z_ARRVAL_P(candidates));
	array_init(return_value);
	if (total == 0 || k == 0) {
		return;
	}

	scores = safe_emalloc(total, sizeof(ir_scored_index), 0);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(candidates), candidate) {
		if (Z_TYPE_P(candidate) != IS_ARRAY) {
			efree(scores);
			zend_argument_value_error(2, "must be an array of sparse vectors");
			RETURN_THROWS();
		}
		scores[i].index = (zend_long) i;
		scores[i].score = ir_sparse_similarity(Z_ARRVAL_P(query), Z_ARRVAL_P(candidate), metric);
		i++;
	} ZEND_HASH_FOREACH_END();

	qsort(scores, total, sizeof(ir_scored_index), ir_score_desc_cmp);
	out_n = (k < (zend_long) total) ? (uint32_t) k : total;

	for (i = 0; i < out_n; i++) {
		zval row;
		array_init(&row);
		add_assoc_long(&row, "index", scores[i].index);
		add_assoc_double(&row, "score", scores[i].score);
		add_next_index_zval(return_value, &row);
	}

	efree(scores);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(ir)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "LinearAlgebra", class_CoralMedia_IR_LinearAlgebra_methods);
	ir_ce_linear_algebra = zend_register_internal_class(&ce);
	ir_ce_linear_algebra->ce_flags |= ZEND_ACC_FINAL;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Text", class_CoralMedia_IR_Text_methods);
	ir_ce_text = zend_register_internal_class(&ce);
	ir_ce_text->ce_flags |= ZEND_ACC_FINAL;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Vectorizer", class_CoralMedia_IR_Vectorizer_methods);
	ir_ce_vectorizer = zend_register_internal_class(&ce);
	ir_ce_vectorizer->ce_flags |= ZEND_ACC_FINAL;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Similarity", class_CoralMedia_IR_Similarity_methods);
	ir_ce_similarity = zend_register_internal_class(&ce);
	ir_ce_similarity->ce_flags |= ZEND_ACC_FINAL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(ir)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ir support", "enabled");
	php_info_print_table_row(2, "version", PHP_IR_VERSION);
	php_info_print_table_end();
}
/* }}} */

zend_module_entry ir_module_entry = {
	STANDARD_MODULE_HEADER,
	"ir",
	ext_functions,
	PHP_MINIT(ir),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ir),
	PHP_IR_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IR
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(ir)
#endif
