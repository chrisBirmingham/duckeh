--TEST--
Test Result print
--EXTENSIONS--
duckdb
--FILE--
<?php

$db = new \DuckDB\DuckDB();
$result = $db->query("SELECT 1 AS a, 'duck' AS b");
$result->print();
?>
--EXPECT--
------------
| a | b    |
------------
| 1 | duck |
------------
