--TEST--
Prepared statement
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TABLE test_data (i INTEGER, b BOOL, f FLOAT);");
$duckDB->query("INSERT INTO test_data VALUES (3, true, 1.1), (5, true, 1.2), (3, false, 1.1), (3, null, 1.2);");

$stmt = $duckDB->prepare("SELECT * FROM test_data WHERE b = $1");

$stmt->bindParam(1, true);
$result = $stmt->execute();

while ($row = $result->fetch()) {
    foreach ($row as $col) {
        var_dump($col);
    }
}

$stmt->bindParam(1, false);
$result = $stmt->execute();

while ($row = $result->fetch()) {
    foreach ($row as $col) {
        var_dump($col);
    }
}

$stmt = $duckDB->prepare('SELECT * FROM test_data WHERE i = $index');
$stmt->bindParam('index', 5);
$result = $stmt->execute();

while ($row = $result->fetch()) {
    foreach ($row as $col) {
        var_dump($col);
    }
}

?>
--EXPECT--
int(3)
bool(true)
float(1.100000023841858)
int(5)
bool(true)
float(1.2000000476837158)
int(3)
bool(false)
float(1.100000023841858)
int(5)
bool(true)
float(1.2000000476837158)
