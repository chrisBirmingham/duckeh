--TEST--
Test string representation for non-customized types (interval)
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT '16 months'::INTERVAL as mycolumn_interval;");

$row = $result->fetch();
var_dump($row['mycolumn_interval']);

?>
--EXPECT--
string(15) "1 year 4 months"
