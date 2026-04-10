--TEST--
Test Struct
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT {'yes': 'duck', 'maybe': 'goose', 'huh': NULL, 'no': 'heron'} AS s;");

$row = $result->fetch();
var_dump($row['s']);

$result = $duckDB->query("SELECT * FROM VALUES ({'yes': 'duck', 'maybe': 'goose'}), ({'yes': 'cua', 'maybe': 'alfred'}) AS s;");

while ($row = $result->fetch()) {
    foreach ($row as $col) {
        var_dump($col);
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
