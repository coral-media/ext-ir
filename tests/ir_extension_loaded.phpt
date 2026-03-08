--TEST--
ir extension is loaded
--EXTENSIONS--
ir
--FILE--
<?php
var_dump(extension_loaded('ir'));
?>
--EXPECT--
bool(true)
