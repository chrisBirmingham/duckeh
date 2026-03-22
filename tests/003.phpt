--TEST--
Create a DuckDB object
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

var_dump($duckDB);

?>
--EXPECT--
object(DuckDB\DuckDB)#1 (0) {
}