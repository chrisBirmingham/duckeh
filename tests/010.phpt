--TEST--
Test iterate rows with integers
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT 1 as mycolumn1, 4 as mycolumn2;");

$row = $result->fetch();
var_dump($row['mycolumn1']);
var_dump($row['mycolumn2']);

?>
--EXPECT--
int(1)
int(4)
