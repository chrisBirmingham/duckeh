--TEST--
Test decimal, blob, bit, time_tz
--EXTENSIONS--
duckdb
--FILE--
<?php

$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT CAST('12.5' AS DECIMAL(4,1)) AS d, BLOB 'abcd' AS b, CAST('1010' AS BIT) AS bit, CAST('12:34:56+02' AS TIME WITH TIME ZONE) AS ttz;");

$row = $result->fetch();

foreach ($row as $col) {
    if ($col instanceof DuckDB\Value\Time) {
        echo "$col\n";
    } else {
        var_dump($col);
    }
}

?>
--EXPECT--
float(12.5)
string(4) "abcd"
string(4) "1010"
12:34:56+02
