--TEST--
Similarity sparse and dense vectors are consistent
--EXTENSIONS--
ir
--FILE--
<?php
$tokens = [
    ["termx", "termx", "termy"],
    ["termy", "termy", "termy", "termy"],
];

$model = CoralMedia\IR\Vectorizer::fit($tokens);
$sparse = CoralMedia\IR\Vectorizer::transform($tokens, $model);
$dense = CoralMedia\IR\Vectorizer::densify($sparse, $model);

var_dump(abs(CoralMedia\IR\Similarity::cosine($sparse[0], $sparse[1]) - CoralMedia\IR\Similarity::cosine($dense[0], $dense[1])) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::euclidean($sparse[0], $sparse[1]) - CoralMedia\IR\Similarity::euclidean($dense[0], $dense[1])) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::pearson($sparse[0], $sparse[1]) - CoralMedia\IR\Similarity::pearson($dense[0], $dense[1])) < 1e-12);
var_dump(abs(CoralMedia\IR\LinearAlgebra::normL2($sparse[0]) - CoralMedia\IR\LinearAlgebra::normL2($dense[0])) < 1e-12);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
