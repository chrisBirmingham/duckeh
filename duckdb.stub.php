<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80100
 */

namespace DuckDB {
    /**
     * @var int
     * @cvalue DUCKDB_ERROR_OUT_OF_RANGE
     */
    const ERROR_OUT_OF_RANGE = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_CONVERSION
     */
    const ERROR_CONVERSION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_UNKNOWN_TYPE
     */
    const ERROR_UNKNOWN_TYPE = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_DECIMAL
     */
    const ERROR_DECIMAL = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_MISMATCH_TYPE
     */
    const ERROR_MISMATCH_TYPE = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_DIVIDE_BY_ZERO
     */
    const ERROR_DIVIDE_BY_ZERO = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_OBJECT_SIZE
     */
    const ERROR_OBJECT_SIZE = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_INVALID_TYPE
     */
    const ERROR_INVALID_TYPE = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_SERIALIZATION
     */
    const ERROR_SERIALIZATION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_TRANSACTION
     */
    const ERROR_TRANSACTION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_NOT_IMPLEMENTED
     */
    const ERROR_NOT_IMPLEMENTED = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_EXPRESSION
     */
    const ERROR_EXPRESSION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_CATALOG
     */
    const ERROR_CATALOG = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_PARSER
     */
    const ERROR_PARSER = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_PLANNER
     */
    const ERROR_PLANNER = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_SCHEDULER
     */
    const ERROR_SCHEDULER = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_EXECUTOR
     */
    const ERROR_EXECUTOR = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_CONSTRAINT
     */
    const ERROR_CONSTRAINT = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_INDEX
     */
    const ERROR_INDEX = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_STAT
     */
    const ERROR_STAT = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_CONNECTION
     */
    const ERROR_CONNECTION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_SYNTAX
     */
    const ERROR_SYNTAX = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_SETTINGS
     */
    const ERROR_SETTINGS = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_BINDER
     */
    const ERROR_BINDER = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_NETWORK
     */
    const ERROR_NETWORK = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_OPTIMIZER
     */
    const ERROR_OPTIMIZER = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_NULL_POINTER
     */
    const ERROR_NULL_POINTER = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_IO
     */
    const ERROR_IO = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_INTERRUPT
     */
    const ERROR_INTERRUPT = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_FATAL
     */
    const ERROR_FATAL = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_INTERNAL
     */
    const ERROR_INTERNAL = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_INVALID_INPUT
     */
    const ERROR_INVALID_INPUT = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_OUT_OF_MEMORY
     */
    const ERROR_OUT_OF_MEMORY = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_PERMISSION
     */
    const ERROR_PERMISSION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_PARAMETER_NOT_RESOLVED
     */
    const ERROR_PARAMETER_NOT_RESOLVED = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_PARAMETER_NOT_ALLOWED
     */
    const ERROR_PARAMETER_NOT_ALLOWED = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_DEPENDENCY
     */
    const ERROR_DEPENDENCY = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_HTTP
     */
    const ERROR_HTTP = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_MISSING_EXTENSION
     */
    const ERROR_MISSING_EXTENSION = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_AUTOLOAD
     */
    const ERROR_AUTOLOAD = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_ERROR_SEQUENCE
     */
    const ERROR_SEQUENCE = UNKNOWN;

    /**
     * @var int
     * @cvalue DUCKDB_INVALID_CONFIGURATION
     */
    const INVALID_CONFIGURATION = UNKNOWN;

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

        public function fetch(): array|false {}

        public function fetchAll(): array {}
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
    const POSITIVE_INFINITY = UNKNOWN;

    /**
     * @var int
     * @cvalue PHP_DUCKDB_NEGATIVE_INFINITY
     */
    const NEGATIVE_INFINITY = UNKNOWN;

    /**
     * @var int
     * @cvalue PHP_DUCKDB_FINITE
     */
    const FINITE = UNKNOWN;

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
