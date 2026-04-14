--TEST--
Test data chunk size
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT * FROM repeat('quack', 1000000);");

$rows = 0;

while ($result->fetch()) {
    $rows++;
}

printf("%s rows", $rows)

?>
--EXPECT--
1000000 rows
