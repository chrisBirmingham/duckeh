--TEST--
Test timestamp
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT '1521-04-23 12:45:17.1234'::TIMESTAMP as mycolumn_date1, '1521-04-17 12:45:17.1234'::TIMESTAMP as mycolumn_date2;");

while ($row = $result->fetch()) {
    foreach ($row as $col) {
        printf("%s\n", $col);
        printf("Date: %s\n", $col->getDate());
        printf("Time: %s\n", $col->getTime());
    }
}

$result = $duckDB->query("SELECT 'infinity'::TIMESTAMP as infinity, '-infinity'::TIMESTAMP as negative_infinity, 'epoch'::TIMESTAMP as finite;");
$row = $result->fetch();

printf("Infinity value: %s\n", $row['infinity']->infinity());
printf("Infinity value: %s\n", $row['negative_infinity']->infinity());
printf("Infinity value: %s\n", $row['finite']->infinity());

?>
--EXPECT--
1521-04-23 12:45:17.1234
Date: 1521-04-23
Time: 12:45:17.123400
1521-04-17 12:45:17.1234
Date: 1521-04-17
Time: 12:45:17.123400
Infinity value: 1
Infinity value: -1
Infinity value: 0
