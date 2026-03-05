--TEST--
Test Connection Failure
--EXTENSIONS--
duckdb
--FILE--
<?php
try {
  new \DuckDB\DuckDB(__DIR__ . "/028.phpt");
} catch (\DuckDB\ConnectionException $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
Failed to initialize duckdb. Failed to open database
