--TEST--
Prepared statement invalid type in execute param
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TABLE test_data (i INTEGER, b BOOL, f FLOAT);");
$duckDB->query("INSERT INTO test_data VALUES (3, true, 1.1), (5, true, 1.2), (3, false, 1.1), (3, null, 1.2);");

$stmt = $duckDB->prepare("SELECT * FROM test_data WHERE b = $1");

try {
    $stmt->execute([1 => []]);
} catch (InvalidArgumentException) {
    echo "Invalid type provided execute\n";
}

try {
    $stmt->bindParam(1, []);
} catch (InvalidArgumentException) {
    echo "Invalid type provided bind\n";
}

?>
--EXPECT--
Invalid type provided execute
Invalid type provided bind
