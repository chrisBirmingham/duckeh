--TEST--
Test iterate rows
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT 'quack' as mycolumn1, 'quick' as mycolumn2;");

$row = $result->fetch();
var_dump($row['mycolumn1']);
var_dump($row['mycolumn2']);
?>
--EXPECT--
string(5) "quack"
string(5) "quick"
