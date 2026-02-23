--TEST--
Test Result print
--EXTENSIONS--
duckdb
--FILE--
<?php
$result = \DuckDB\DuckDB::sql("SELECT 1 AS a, 'duck' AS b");
$result->print();
?>
--EXPECT--
------------
| a | b    |
------------
| 1 | duck |
------------
