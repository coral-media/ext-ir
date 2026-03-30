--TEST--
Vectorizer fit transform and fitTransform work
--EXTENSIONS--
ir
--FILE--
<?php
$tokens = [
    ["termx", "termx", "termy"],
    ["termy", "termy", "termy", "termy"],
];

$model = CoralMedia\IR\Vectorizer::fit($tokens);
$matrix = CoralMedia\IR\Vectorizer::transform($tokens, $model);
$fitTransform = CoralMedia\IR\Vectorizer::fitTransform($tokens);
$dense = CoralMedia\IR\Vectorizer::densify($matrix, $model);

var_dump(isset($model["documents"], $model["vocabulary"], $model["df"], $model["idf"]));
var_dump($model["documents"] === 2);
var_dump($model["df"] === ["termx" => 1, "termy" => 2]);
var_dump(abs($model["idf"]["termx"] - (log(1.5) + 1.0)) < 1e-12);
var_dump(abs($model["idf"]["termy"] - 1.0) < 1e-12);

var_dump(abs($matrix[0]["termx"] - (2 * (log(1.5) + 1.0))) < 1e-12);
var_dump(abs($matrix[0]["termy"] - 1.0) < 1e-12);
var_dump(isset($matrix[1]["termx"]) === false);
var_dump(abs($matrix[1]["termy"] - 4.0) < 1e-12);

var_dump($fitTransform["model"]["df"] === $model["df"]);
var_dump(abs($fitTransform["matrix"][0]["termx"] - $matrix[0]["termx"]) < 1e-12);
var_dump(count($dense) === 2);
var_dump(abs($dense[0][0] - $matrix[0]["termx"]) < 1e-12);
var_dump(abs($dense[0][1] - $matrix[0]["termy"]) < 1e-12);
var_dump($dense[1][0] === 0.0);
var_dump(abs($dense[1][1] - $matrix[1]["termy"]) < 1e-12);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
