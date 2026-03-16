--TEST--
Test Connection Failure invalid config
--EXTENSIONS--
duckdb
--FILE--
<?php
try {
  new \DuckDB\DuckDB(options: ['threads' => 7]);
} catch (\InvalidArgumentException $e) {
  echo $e->getMessage() . "\n";
}

# Found in testing that even though the api says it will fail on setting an invalid option, the config struct allowed an invalid option

try {
  new \DuckDB\DuckDB(options: ['threa' => '']);
} catch (\InvalidArgumentException|\DuckDB\ConnectionException $e) {
  echo "Invalid option\n";
}
?>
--EXPECT--
Invalid value for config option 'threads'. Value must be a string
Invalid option
