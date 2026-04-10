--TEST--
Test consecutive iterate rows
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT 'quack' as mycolumn1, 'quick' as mycolumn2;");

$row = $result->fetch();
var_dump($row['mycolumn1']);
var_dump($row['mycolumn2']);

$result = $duckDB->query("SELECT 'queck' as mycolumn1, 'quock' as mycolumn2;");

$row = $result->fetch();
var_dump($row['mycolumn1']);
var_dump($row['mycolumn2']);

$result = $duckDB->query("SELECT * FROM repeat('quack', 1000000);");

$rows = 0;

while ($result->fetch()) {
    $rows++;
}

printf("%s rows", $rows)

?>
--EXPECT--
string(5) "quack"
string(5) "quick"
string(5) "queck"
string(5) "quock"
1000000 rows
