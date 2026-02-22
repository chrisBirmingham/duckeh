--TEST--
Test hugeint and ubigint
--EXTENSIONS--
duckdb
--FILE--
<?php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT CAST('170141183460469231731687303715884105727' AS HUGEINT) AS h, CAST('340282366920938463463374607431768211455' AS UHUGEINT) AS uh, CAST('18446744073709551615' AS UBIGINT) AS ub;");

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
string(39) "170141183460469231731687303715884105727"
string(39) "340282366920938463463374607431768211455"
string(20) "18446744073709551615"
