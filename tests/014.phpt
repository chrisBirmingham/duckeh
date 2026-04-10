--TEST--
Test string representation for non-customized types (uuid)
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT '9180eef3-9683-4568-bc19-a6968bc5e7b0'::UUID as mycolumn_uuid1;");

$row = $result->fetch();
var_dump($row['mycolumn_uuid1']);

?>
--EXPECT--
string(36) "9180eef3-9683-4568-bc19-a6968bc5e7b0"
