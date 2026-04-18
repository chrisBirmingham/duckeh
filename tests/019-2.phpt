--TEST--
Test Union
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$duckDB->query("CREATE TABLE tbl1 (u UNION(num INTEGER, str VARCHAR))");
$duckDB->query("INSERT INTO tbl1 VALUES (1), ('two'), (union_value(str := 'three'))");

$result = $duckDB->query("SELECT u FROM tbl1");

while ($row = $result->fetch()) {
  var_dump($row['u']);
}

$result = $duckDB->query("SELECT union_extract(u, 'str') AS str FROM tbl1");

while ($row = $result->fetch()) {
  var_dump($row['str']);
}

$result = $duckDB->query("SELECT union_tag(u) AS t FROM tbl1");

while ($row = $result->fetch()) {
  var_dump($row['t']);
}

?>
--EXPECT--
int(1)
string(3) "two"
string(5) "three"
NULL
string(3) "two"
string(5) "three"
string(3) "num"
string(3) "str"
string(3) "str"
