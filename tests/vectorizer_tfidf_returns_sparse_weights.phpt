--TEST--
Vectorizer tfIdf returns sparse weights
--EXTENSIONS--
ir
--FILE--
<?php
$tokens = [
    ["termx", "termx", "termy"],
    ["termy", "termy", "termy", "termy"],
];

$tfidf = CoralMedia\IR\Vectorizer::tfIdf($tokens);

$idfX = log((2 + 1) / (1 + 1)) + 1; // log(1.5)+1
$idfY = log((2 + 1) / (2 + 1)) + 1; // 1

var_dump(abs($tfidf[0]["termx"] - (2 * $idfX)) < 1e-12);
var_dump(abs($tfidf[0]["termy"] - (1 * $idfY)) < 1e-12);
var_dump(isset($tfidf[1]["termx"]) === false);
var_dump(abs($tfidf[1]["termy"] - (4 * $idfY)) < 1e-12);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
