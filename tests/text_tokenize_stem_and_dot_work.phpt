--TEST--
namespaced classes and static methods work
--EXTENSIONS--
ir
--FILE--
<?php
var_dump(class_exists('CoralMedia\\IR\\LinearAlgebra'));
var_dump(class_exists('CoralMedia\\IR\\Text'));

var_dump(CoralMedia\IR\LinearAlgebra::dot([1.0, 2.0], [3.0, 4.0]) === 11.0);
var_dump(abs(CoralMedia\IR\LinearAlgebra::normL2([3.0, 4.0]) - 5.0) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::pearson([1.0, 2.0], [2.0, 4.0]) - 1.0) < 1e-12);
var_dump(CoralMedia\IR\Similarity::pearson([1.0, 1.0], [2.0, 3.0]) === 0.0);
var_dump(abs(CoralMedia\IR\Similarity::cosine([1.0, 2.0], [2.0, 4.0]) - 1.0) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::euclidean([1.0, 2.0], [2.0, 4.0]) - 0.3090169943749474) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::cosine(["a" => 1.0, "c" => 2.0], ["b" => 3.0, "c" => 4.0, "d" => 1.0]) - (8.0 / (sqrt(5.0) * sqrt(26.0)))) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::euclidean(["a" => 1.0, "c" => 2.0], ["b" => 3.0, "c" => 4.0, "d" => 1.0]) - (1.0 / (1.0 + sqrt(15.0)))) < 1e-12);
var_dump(abs(CoralMedia\IR\Similarity::pearson(["a" => 1.0, "c" => 2.0], ["b" => 3.0, "c" => 4.0, "d" => 1.0]) - (8.0 / sqrt(440.0))) < 1e-12);
var_dump(CoralMedia\IR\Text::tokenize(
    ["Canción RÁPIDA en casa", "La cancion en vivo"],
    true,
    true,
    false,
    "english",
    '/\s+/u',
    ["canción", "en"]
) === [["rapida", "casa"], ["la", "vivo"]]);
$latin1 = "Canci\xf3n en CASA";
var_dump(CoralMedia\IR\Text::tokenize(
    [$latin1],
    true,
    true,
    false,
    "english",
    '/\s+/u',
    ["canción", "en"]
) === [["casa"]]);
var_dump(CoralMedia\IR\Text::stem("running") === "run");
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
