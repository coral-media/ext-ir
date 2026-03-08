--TEST--
Vectorizer frequency and vocabulary from tokenized items
--EXTENSIONS--
ir
--FILE--
<?php
$tokens = [
    ["termx", "termx", "termy"],
    ["termy", "termy", "termy", "termy"],
];

$tf = CoralMedia\IR\Vectorizer::frequency($tokens);
$df = CoralMedia\IR\Vectorizer::vocabulary($tokens);

var_dump($tf === [
    ["termx" => 2, "termy" => 1],
    ["termy" => 4],
]);
var_dump($df === [
    "termx" => 1,
    "termy" => 2,
]);
?>
--EXPECT--
bool(true)
bool(true)
