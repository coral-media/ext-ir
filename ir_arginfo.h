/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f59b3f22d1d7be384496f669ddf7ac94c8c802cc */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ir_version, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_LinearAlgebra_dot, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, y, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_LinearAlgebra_normL2, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Text_tokenize, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, texts, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lowercase, _IS_BOOL, 0, "true")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stripDiacritics, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stem, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stemLanguage, IS_STRING, 0, "\"english\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pattern, IS_STRING, 0, "\'/\\s+/u\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, stopwords, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Text_stem, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, word, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, language, IS_STRING, 0, "\"english\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Vectorizer_frequency, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, tokenizedItems, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CoralMedia_IR_Vectorizer_vocabulary arginfo_class_CoralMedia_IR_Vectorizer_frequency

#define arginfo_class_CoralMedia_IR_Vectorizer_fit arginfo_class_CoralMedia_IR_Vectorizer_frequency

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Vectorizer_transform, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, tokenizedItems, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, model, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CoralMedia_IR_Vectorizer_fitTransform arginfo_class_CoralMedia_IR_Vectorizer_frequency

#define arginfo_class_CoralMedia_IR_Vectorizer_tfIdf arginfo_class_CoralMedia_IR_Vectorizer_frequency

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Vectorizer_densify, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, matrix, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, model, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CoralMedia_IR_Similarity_pearson arginfo_class_CoralMedia_IR_LinearAlgebra_dot

#define arginfo_class_CoralMedia_IR_Similarity_cosine arginfo_class_CoralMedia_IR_LinearAlgebra_dot

#define arginfo_class_CoralMedia_IR_Similarity_euclidean arginfo_class_CoralMedia_IR_LinearAlgebra_dot

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Ranking_nearest, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, candidates, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, metric, IS_STRING, 0, "\"cosine\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Ranking_topK, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, candidates, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, k, IS_LONG, 0, "5")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, metric, IS_STRING, 0, "\"cosine\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_CoralMedia_IR_Ranking_bm25, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, queryTokens, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, tokenizedItems, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, k1, IS_DOUBLE, 0, "1.5")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, b, IS_DOUBLE, 0, "0.75")
ZEND_END_ARG_INFO()


ZEND_FUNCTION(ir_version);
ZEND_METHOD(CoralMedia_IR_LinearAlgebra, dot);
ZEND_METHOD(CoralMedia_IR_LinearAlgebra, normL2);
ZEND_METHOD(CoralMedia_IR_Text, tokenize);
ZEND_METHOD(CoralMedia_IR_Text, stem);
ZEND_METHOD(CoralMedia_IR_Vectorizer, frequency);
ZEND_METHOD(CoralMedia_IR_Vectorizer, vocabulary);
ZEND_METHOD(CoralMedia_IR_Vectorizer, fit);
ZEND_METHOD(CoralMedia_IR_Vectorizer, transform);
ZEND_METHOD(CoralMedia_IR_Vectorizer, fitTransform);
ZEND_METHOD(CoralMedia_IR_Vectorizer, tfIdf);
ZEND_METHOD(CoralMedia_IR_Vectorizer, densify);
ZEND_METHOD(CoralMedia_IR_Similarity, pearson);
ZEND_METHOD(CoralMedia_IR_Similarity, cosine);
ZEND_METHOD(CoralMedia_IR_Similarity, euclidean);
ZEND_METHOD(CoralMedia_IR_Ranking, nearest);
ZEND_METHOD(CoralMedia_IR_Ranking, topK);
ZEND_METHOD(CoralMedia_IR_Ranking, bm25);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(ir_version, arginfo_ir_version)
	ZEND_FE_END
};


static const zend_function_entry class_CoralMedia_IR_LinearAlgebra_methods[] = {
	ZEND_ME(CoralMedia_IR_LinearAlgebra, dot, arginfo_class_CoralMedia_IR_LinearAlgebra_dot, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_LinearAlgebra, normL2, arginfo_class_CoralMedia_IR_LinearAlgebra_normL2, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_CoralMedia_IR_Text_methods[] = {
	ZEND_ME(CoralMedia_IR_Text, tokenize, arginfo_class_CoralMedia_IR_Text_tokenize, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Text, stem, arginfo_class_CoralMedia_IR_Text_stem, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_CoralMedia_IR_Vectorizer_methods[] = {
	ZEND_ME(CoralMedia_IR_Vectorizer, frequency, arginfo_class_CoralMedia_IR_Vectorizer_frequency, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Vectorizer, vocabulary, arginfo_class_CoralMedia_IR_Vectorizer_vocabulary, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Vectorizer, fit, arginfo_class_CoralMedia_IR_Vectorizer_fit, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Vectorizer, transform, arginfo_class_CoralMedia_IR_Vectorizer_transform, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Vectorizer, fitTransform, arginfo_class_CoralMedia_IR_Vectorizer_fitTransform, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Vectorizer, tfIdf, arginfo_class_CoralMedia_IR_Vectorizer_tfIdf, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Vectorizer, densify, arginfo_class_CoralMedia_IR_Vectorizer_densify, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_CoralMedia_IR_Similarity_methods[] = {
	ZEND_ME(CoralMedia_IR_Similarity, pearson, arginfo_class_CoralMedia_IR_Similarity_pearson, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Similarity, cosine, arginfo_class_CoralMedia_IR_Similarity_cosine, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Similarity, euclidean, arginfo_class_CoralMedia_IR_Similarity_euclidean, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};


static const zend_function_entry class_CoralMedia_IR_Ranking_methods[] = {
	ZEND_ME(CoralMedia_IR_Ranking, nearest, arginfo_class_CoralMedia_IR_Ranking_nearest, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Ranking, topK, arginfo_class_CoralMedia_IR_Ranking_topK, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(CoralMedia_IR_Ranking, bm25, arginfo_class_CoralMedia_IR_Ranking_bm25, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_CoralMedia_IR_LinearAlgebra(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "LinearAlgebra", class_CoralMedia_IR_LinearAlgebra_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_CoralMedia_IR_Text(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Text", class_CoralMedia_IR_Text_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_CoralMedia_IR_Vectorizer(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Vectorizer", class_CoralMedia_IR_Vectorizer_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_CoralMedia_IR_Similarity(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Similarity", class_CoralMedia_IR_Similarity_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}

static zend_class_entry *register_class_CoralMedia_IR_Ranking(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "CoralMedia\\IR", "Ranking", class_CoralMedia_IR_Ranking_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_FINAL;

	return class_entry;
}
