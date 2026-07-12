--TEST--
Test Time
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$row = $duckDB->query("SELECT '11:30:00.123456'::TIME as time, '11:30:00'::TIME as time2,CAST('12:34:56+02' AS TIME WITH TIME ZONE) AS ttz")->fetch();
var_dump($row['time']);
var_dump($row['time2']);
var_dump($row['ttz']);

?>
--EXPECT--
string(15) "11:30:00.123456"
string(8) "11:30:00"
string(14) "12:34:56+02:00"
