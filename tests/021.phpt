--TEST--
Test list, array, map, enum
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TYPE mood AS ENUM ('sad', 'ok', 'happy');");

$result = $duckDB->query("SELECT [1,2,3]::INTEGER[] AS l, [1,2,3]::INTEGER[3] AS a, MAP(['a','b'], [1,2]) AS m, 'happy'::mood AS e;");

$row = $result->fetch();
var_dump($row['l']);
var_dump($row['a']);
var_dump($row['m']);
var_dump($row['e']);

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
