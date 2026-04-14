--TEST--
Test strings
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT 'quack' as mycolumn1, 'this is a longer string, more than 12 characters' as mycolumn2;");

$row = $result->fetch();
var_dump($row['mycolumn1']);
var_dump($row['mycolumn2']);

?>
--EXPECT--
string(5) "quack"
string(48) "this is a longer string, more than 12 characters"
