--TEST--
Test Date
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT '1992-09-20'::DATE as mycolumn1;");
$data = $result->fetch()['mycolumn1'];

var_dump($data);

$result = $duckDB->query("SELECT 'infinity'::DATE as infinity, '-infinity'::DATE as negative_infinity, 'epoch'::DATE as finite;");
$row = $result->fetch();

var_dump($row['infinity']);
var_dump($row['negative_infinity']);
var_dump($row['finite']);

?>
--EXPECT--
string(10) "1992-09-20"
string(8) "infinity"
string(9) "-infinity"
string(10) "1970-01-01"
