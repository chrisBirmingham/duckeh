--TEST--
Test appender
--EXTENSIONS--
duckdb
--FILE--
<?php
$db = new \DuckDB\DuckDB();
$res = $db->query('CREATE TABLE people (id INTEGER, name VARCHAR)');
$appender = $db->append('people');

$appender->appendRow([1, 'Duck']);
$appender->flush();

$db->query('SELECT * FROM people')->print();

for ($i = 2; $i <= 10; $i++) {
    $appender->appendRow([$i, 'Duck' . $i]);
}

$appender->flush();

$db->query('SELECT * FROM people')->print();

?>
--EXPECT--
-------------
| id | name |
-------------
| 1  | Duck |
-------------
---------------
| id | name   |
---------------
| 1  | Duck   |
---------------
| 2  | Duck2  |
---------------
| 3  | Duck3  |
---------------
| 4  | Duck4  |
---------------
| 5  | Duck5  |
---------------
| 6  | Duck6  |
---------------
| 7  | Duck7  |
---------------
| 8  | Duck8  |
---------------
| 9  | Duck9  |
---------------
| 10 | Duck10 |
---------------
