--TEST--
Prepared statement params in execute
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TABLE test_data (i INTEGER, b BOOL, f FLOAT);");
$duckDB->query("INSERT INTO test_data VALUES (3, true, 1.1), (5, true, 1.2), (3, false, 1.1), (3, null, 1.2);");

$stmt = $duckDB->prepare("SELECT * FROM test_data WHERE b = $1");

$result = $stmt->execute([1 => true]);

$columns = $result->columnCount();
while ($dataChunk = $result->fetchChunk()) {
    $rows = $dataChunk->getSize();
    for ($i = 0; $i < $columns; $i++) {
        $vector = $dataChunk->getVector($i);
        for ($r = 0; $r < $rows; $r++) {
            $data = $vector->getData($r);
            var_dump($data);
        }
    }
}

$result = $stmt->execute([1 => false]);

$columns = $result->columnCount();
while ($dataChunk = $result->fetchChunk()) {
    $rows = $dataChunk->getSize();
    for ($i = 0; $i < $columns; $i++) {
        $vector = $dataChunk->getVector($i);
        for ($r = 0; $r < $rows; $r++) {
            $data = $vector->getData($r);
            var_dump($data);
        }
    }
}

$stmt = $duckDB->prepare('SELECT * FROM test_data WHERE i = $index');
$result = $stmt->execute(['index' => 5]);

$columns = $result->columnCount();
while ($dataChunk = $result->fetchChunk()) {
    $rows = $dataChunk->getSize();
    for ($i=0; $i<$columns; $i++) {
        $vector = $dataChunk->getVector($i);
        for ($r=0; $r<$rows; $r++) {
            $data = $vector->getData($r);
            var_dump($data);
        }
    }
}

?>
--EXPECT--
int(3)
int(5)
bool(true)
bool(true)
float(1.100000023841858)
float(1.2000000476837158)
int(3)
bool(false)
float(1.100000023841858)
int(5)
bool(true)
float(1.2000000476837158)
