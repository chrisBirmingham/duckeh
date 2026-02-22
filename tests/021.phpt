--TEST--
Test list, array, map, enum
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TYPE mood AS ENUM ('sad', 'ok', 'happy');");

$result = $duckDB->query("SELECT [1,2,3]::INTEGER[] AS l, [1,2,3]::INTEGER[3] AS a, MAP(['a','b'], [1,2]) AS m, 'happy'::mood AS e;");

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
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
string(5) "happy"
