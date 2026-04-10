--TEST--
Test decimal (float, double)
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT 1.1::FLOAT as mycolumn1, 4.1::DOUBLE as mycolumn2;");

$row = $result->fetch();
var_dump($row['mycolumn1']);
var_dump($row['mycolumn2']);

?>
--EXPECT--
float(1.100000023841858)
float(4.1)
