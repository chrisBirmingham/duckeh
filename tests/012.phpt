--TEST--
Test boolean
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT true as mycolumn_true, false as mycolumn_false;");

$row = $result->fetch();
var_dump($row['mycolumn_true']);
var_dump($row['mycolumn_false']);

?>
--EXPECT--
bool(true)
bool(false)
