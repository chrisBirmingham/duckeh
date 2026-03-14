--TEST--
Prepared statement out of bounds
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TABLE test_data (i INTEGER, b BOOL, f FLOAT);");
$duckDB->query("INSERT INTO test_data VALUES (3, true, 1.1), (5, true, 1.2), (3, false, 1.1), (3, null, 1.2);");

$stmt = $duckDB->prepare("SELECT * FROM test_data WHERE b = $1");

try {
    $stmt->bindParam(0, true);
} catch (OutOfBoundsException) {
    echo "Out of bounds less than thrown\n";
}

try {
    $stmt->bindParam(2, true);
} catch (OutOfBoundsException) {
    echo "Out of bounds more than thrown\n";
}

try {
    $stmt->bindParam('quack', true);
} catch (OutOfBoundsException) {
    echo "Out of bounds string index thrown\n";
}

?>
--EXPECT--
Out of bounds less than thrown
Out of bounds more than thrown
Out of bounds string index thrown
