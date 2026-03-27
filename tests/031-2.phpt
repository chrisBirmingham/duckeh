--TEST--
Test appender test clear
--EXTENSIONS--
duckdb
--FILE--
<?php
$db = new \DuckDB\DuckDB();
$res = $db->query('CREATE TABLE people (id INTEGER, name VARCHAR)');
$appender = $db->append('people');

for ($i = 0; $i < 10; $i++) {
    $appender->appendRow([$i, 'Duck' . $i]);
}

$appender->clear();
$appender->flush();

$db->query('SELECT * FROM people')->print();

?>
--EXPECT--
-------------
| id | name |
-------------
