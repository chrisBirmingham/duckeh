--TEST--
Test Connection Failure
--EXTENSIONS--
duckdb
--FILE--
<?php
try {
  new \DuckDB\DuckDB(__DIR__ . "/028.phpt");
} catch (\DuckDB\ConnectionException $e) {
  echo "Connection exception thrown";
}
?>
--EXPECT--
Connection exception thrown
