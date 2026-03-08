--TEST--
ir_version returns non empty string
--EXTENSIONS--
ir
--FILE--
<?php
$v = ir_version();
var_dump(is_string($v));
var_dump(strlen($v) > 0);
?>
--EXPECT--
bool(true)
bool(true)
