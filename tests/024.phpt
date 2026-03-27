--TEST--
Test DuckDB::sql
--EXTENSIONS--
duckdb
--FILE--
<?php
$result = \DuckDB\DuckDB::sql("SELECT 42 AS value");

$chunk = $result->fetchChunk();
$vector = $chunk->getVector(0);
var_dump($vector->getData(0));

var_dump($result->fetchChunk());
?>
--EXPECT--
int(42)
bool(false)
