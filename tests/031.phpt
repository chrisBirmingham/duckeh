--TEST--
Test appender
--EXTENSIONS--
duckdb
--FILE--
<?php
$db = new \DuckDB\DuckDB();
$res = $db->query('CREATE TABLE people (id INTEGER, name VARCHAR, dob DATE)');
$appender = $db->append('people');

$appender->appendRow([1, 'Duck', '1995-01-01']);
$appender->flush();

foreach ($db->query('SELECT * FROM people')->fetchAll() as $row) {
    echo "{$row['id']},{$row['name']},{$row['dob']}\n";
}

for ($i = 2; $i <= 10; $i++) {
    $appender->appendRow([$i, 'Duck' . $i, sprintf('1995-01-%02d', $i)]);
}

$appender->flush();

ini_set('duckdb.appender_max_buffer', 5);

for ($i = 1; $i <= 5; $i++) {
    $appender->appendRow([$i + 10, 'Duck' . $i + 10, sprintf('1995-01-%02d', $i + 10)]);
}

foreach ($db->query('SELECT * FROM people')->fetchAll() as $row) {
    echo "{$row['id']},{$row['name']},{$row['dob']}\n";
}

?>
--EXPECT--
1,Duck,1995-01-01
1,Duck,1995-01-01
2,Duck2,1995-01-02
3,Duck3,1995-01-03
4,Duck4,1995-01-04
5,Duck5,1995-01-05
6,Duck6,1995-01-06
7,Duck7,1995-01-07
8,Duck8,1995-01-08
9,Duck9,1995-01-09
10,Duck10,1995-01-10
11,Duck11,1995-01-11
12,Duck12,1995-01-12
13,Duck13,1995-01-13
14,Duck14,1995-01-14
15,Duck15,1995-01-15
