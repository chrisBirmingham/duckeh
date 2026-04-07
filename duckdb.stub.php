<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80100
 */

namespace DuckDB {
    /**
     * @strict-properties
     */
    class DuckDBException extends \Exception {}

    /**
     * @strict-properties
     */
    class ConnectionException extends DuckDBException {}

    /**
     * @strict-properties
     */
    class QueryException extends DuckDBException {}

    /**
     * @strict-properties
     */
    class AppendException extends DuckDBException {}

    /**
     * @not-serializable
     */
    class DuckDB
    {
        /**
         * @throws ConnectionException
         */
        public function __construct(?string $path = null, ?array $options = null) {}

        /**
         * @throws QueryException
         */
        public function query(string $query): Result {}

        /**
         * @throws QueryException
         */
        public function prepare(string $query): PreparedStatement {}

        /**
         * @throws AppendException
         */
        public function append(string $table, ?string $schema = null, ?string $catalogue = null): Appender {}
    }

    /**
     * @not-serializable
     */
    class Result
    {
        public function columnCount(): int {}

        public function rowCount(): int {}

        public function fetchChunk(): DataChunk|false {}

        /**
         * @deprecated
         */
        public function print(): void {}

        public function fetch(): array|false {}

        public function fetchAll(): array {}
    }

    /**
     * @not-serializable
     */
    class DataChunk
    {
        public function getSize(): int {}

        public function getVector(int $columnIndex): Vector {}
    }

    /**
     * @not-serializable
     */
    class Vector
    {
        public function getData(int $rowIndex): mixed {}
    }

    /**
     * @not-serializable
     */
    class PreparedStatement
    {

        /**
         * @throws \OutOfBoundsException
         * @throws \InvalidArgumentException
         * @throws QueryException
         */
        public function bindParam(string|int $param, mixed $val): void {}

        /**
         * @throws \OutOfBoundsException
         * @throws \InvalidArgumentException
         * @throws QueryException
         */
        public function execute(?array $params = null): Result {}
    }

    /**
     * @not-serializable
     */
    class Appender
    {
        /**
         * @throws AppendException
         * @throws \InvalidArgumentException
         */
        public function appendRow(array $row): void {}

        /**
         * @throws AppendException
         */
        public function flush(): void {}

        /**
         * @throws AppendException
         */
        public function clear(): void {}
    }
}

/**
 * @generate-class-entries
 */
namespace DuckDB\Value {
    /**
     * @var int
     * @cvalue PHP_DUCKDB_POSITIVE_INFINITY
     */
    const POSITIVE_INFINITY = 1;

    /**
     * @var int
     * @cvalue PHP_DUCKDB_NEGATIVE_INFINITY
     */
    const NEGATIVE_INFINITY = -1;

    /**
     * @var int
     * @cvalue PHP_DUCKDB_FINITE
     */
    const FINITE = 0;

    /**
     * @not-serializable
     */
    class Timestamp implements \Stringable
    {
        public function infinity(): int {}

        public function getDate(): Date {}

        public function getTime(): Time {}

        public function __toString(): string {}
    }

    /**
     * @not-serializable
     */
    class Date implements \Stringable
    {
        public function infinity(): int {}

        public function getYear(): int {}

        public function getMonth(): int {}

        public function getDay(): int {}

        public function getDays(): int {}

        public function __toString(): string {}
    }

    /**
     * @not-serializable
     */
    class Time implements \Stringable
    {
        public function getTotalMicroseconds(): int {}

        public function getHour(): int {}

        public function getMinutes(): int {}

        public function getSeconds(): int {}

        public function getMicroseconds(): int {}

        public function __toString(): string {}
    }
}
