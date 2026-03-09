--TEST--
Similarity nearest and topK work
--EXTENSIONS--
ir
--FILE--
<?php
$query = ["x" => 1.0, "y" => 1.0];
$candidates = [
    ["x" => 1.0, "y" => 0.0],
    ["x" => 0.9, "y" => 0.9],
    ["x" => 0.0, "y" => 1.0],
];

$nearest = CoralMedia\IR\Similarity::nearest($query, $candidates, "cosine");
$topK = CoralMedia\IR\Similarity::topK($query, $candidates, 2, "cosine");
$nearestPearson = CoralMedia\IR\Similarity::nearest(
    ["x" => 1.0, "y" => 2.0],
    [
        ["x" => 2.0, "y" => 4.0],
        ["x" => 2.0, "y" => 1.0],
    ],
    "pearson"
);

var_dump($nearest === 1);
var_dump(count($topK) === 2);
var_dump($topK[0]["index"] === 1);
var_dump($topK[0]["score"] >= $topK[1]["score"]);
var_dump($nearestPearson === 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
