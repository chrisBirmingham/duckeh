--TEST--
Test Date
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT '1992-09-20'::DATE as mycolumn1;");
$data = $result->fetch()['mycolumn1'];

printf("Class: %s\n", get_class($data));
printf("Year: %s\n", $data->getYear());
printf("Month: %s\n", $data->getMonth());
printf("Day: %s\n", $data->getDay());
printf("Date string: %s\n", $data);
printf("Days since 1970-01-01: %s\n", $data->getDays());

$result = $duckDB->query("SELECT 'infinity'::DATE as infinity, '-infinity'::DATE as negative_infinity, 'epoch'::DATE as finite;");
$row = $result->fetch();

printf("Infinity value: %s\n", $row['infinity']->infinity());
printf("Infinity value: %s\n", $row['negative_infinity']->infinity());
printf("Infinity value: %s\n", $row['finite']->infinity());

?>
--EXPECT--
Class: DuckDB\Value\Date
Year: 1992
Month: 9
Day: 20
Date string: 1992-09-20
Days since 1970-01-01: 8298
Infinity value: 1
Infinity value: -1
Infinity value: 0
