--TEST--
Test timestamp
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT '1521-04-23 12:45:17.1234'::TIMESTAMP as timestamp");
var_dump($result->fetch()['timestamp']);

$result = $duckDB->query("SELECT 'infinity'::TIMESTAMP as infinity, '-infinity'::TIMESTAMP as negative_infinity, 'epoch'::TIMESTAMP as finite;");
$row = $result->fetch();

var_dump($row['infinity']);
var_dump($row['negative_infinity']);
var_dump($row['finite']);

?>
--EXPECT--
string(24) "1521-04-23 12:45:17.1234"
string(8) "infinite"
string(9) "-infinite"
string(19) "1970-01-01 00:00:00"
