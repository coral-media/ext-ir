--TEST--
Ranking bm25 scores tokenized items
--EXTENSIONS--
ir
--FILE--
<?php
$query = ["quick", "fox"];
$items = [
    ["quick", "fox"],
    ["quick", "quick", "dog"],
    ["neural", "search"],
];

$scores = CoralMedia\IR\Ranking::bm25($query, $items);

var_dump(count($scores) === 3);
var_dump($scores[0] > $scores[1]);
var_dump($scores[1] > $scores[2]);
var_dump($scores[2] == 0.0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
