--TEST--
Test Struct
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT {'yes': 'duck', 'maybe': 'goose', 'huh': NULL, 'no': 'heron'} AS s;");

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

$result = $duckDB->query("SELECT * FROM VALUES ({'yes': 'duck', 'maybe': 'goose'}), ({'yes': 'cua', 'maybe': 'alfred'}) AS s;");
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

?>
--EXPECT--
array(4) {
  ["yes"]=>
  string(4) "duck"
  ["maybe"]=>
  string(5) "goose"
  ["huh"]=>
  NULL
  ["no"]=>
  string(5) "heron"
}
array(2) {
  ["yes"]=>
  string(4) "duck"
  ["maybe"]=>
  string(5) "goose"
}
array(2) {
  ["yes"]=>
  string(3) "cua"
  ["maybe"]=>
  string(6) "alfred"
}
