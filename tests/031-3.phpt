--TEST--
Test fetchAll function
--EXTENSIONS--
duckdb
--FILE--
<?php
$db = new \DuckDB\DuckDB(__DIR__ . '/Star_Trek-Season_1.csv');
$res = $db->query('SELECT episode_num FROM "Star_Trek-Season_1" limit 10')->fetchAll();

foreach ($res as $row) {
    echo $row['episode_num'] . "\n" ;
}
?>
--EXPECT--
0
1
2
3
4
5
6
7
8
9
