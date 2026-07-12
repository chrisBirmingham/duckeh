--TEST--
Test decimal, blob, bit
--EXTENSIONS--
duckdb
--FILE--
<?php

$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT CAST('12.5' AS DECIMAL(4,1)) AS d, BLOB 'abcd' AS b, CAST('1010' AS BIT) AS bit");

$row = $result->fetch();

foreach ($row as $col) {
    var_dump($col);
}

?>
--EXPECT--
float(12.5)
string(4) "abcd"
string(4) "1010"
