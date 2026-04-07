--TEST--
Check you can't directly create classes
--EXTENSIONS--
duckdb
--FILE--
<?php

$classes = [
  'result' => DuckDB\Result::class,
  'prepared' => DuckDB\PreparedStatement::class,
  'append' => DuckDB\Appender::class,
  'chunk' => DuckDB\DataChunk::class,
  'vector' => DuckDB\Vector::class,
  'timestamp' => DuckDB\Value\Timestamp::class,
  'date' => DuckDB\Value\Date::class,
  'time' => DuckDB\Value\Time::class
];

foreach ($classes as $name => $cls) {
  try {
    new $cls();
  } catch (Throwable) {
    echo "Can't make $name\n";
  }
}

?>
--EXPECT--
Can't make result
Can't make prepared
Can't make append
Can't make chunk
Can't make vector
Can't make timestamp
Can't make date
Can't make time
