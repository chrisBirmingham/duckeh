# Duckeh - DuckDB for PHP

A PHP extension for the [DuckDB](https://duckdb.org/) API

This is a fork of [Daniel Hernández Marín](https://github.com/dhernandez) original work found [here](https://github.com/satur-io/duckdb)

> [!CAUTION]
> This project is in an early development stage and there are probably a tonne of bugs. Nevertheless, any contribution will be welcome.

## Prerequisites

* libduckdb 1.5.0 and above
* PHP 8.1 and above

## Installation (PIE)

You can install this extension using [PIE](https://github.com/php/pie):

```sh
pie install intermaterium/duckeh
```

## Installation (Build from source)

You can also use the old phpize method to install the extension

```sh
git clone --recursive --depth=1 https://github.com/chrisBirmingham/duckeh.git
cd duckeh
phpize
./configure
make
make install
```

## Configuration

Then enable the extension, add this to your PHP INI file

```ini
extension=duckdb
```

## Usage

### Creation

A new in-memory database can be created like so:

```php
$db = new \DuckDB\DuckDB();
```

You can attach to a store by specifying a path like so.

```php
$db = new \DuckDB\DuckDB(__DIR__ '/Star_Trek-Season_1.csv');
```

A `ConnectionException` is thrown if the class fails to initialise.

### Running Queries

#### Simple

You can run queries via the `query` method:

```php
$db = new \DuckDB\DuckDB();
$res = $db->query('SELECT * FROM duck');
```

The query method returns a `Result` class or throws a `QueryException` if the query fails.

#### Prepared Statements

Prepared statements are created via the `prepare` method:

```php
$db = new \DuckDB\DuckDB();

# Named (`$1`, `$quack`) and unnamed (`?`) parameters are supported.
$stmt = $db->prepare('SELECT * FROM duck where quack = $1');

# Bind a variable to the parameter
$stmt->bindParam(1, "honk");

# Execute the prepared statement
$res = $stmt->execute();
```

On success, a `Result` class is returned otherwise a `QueryException` is thrown if the query fails. If a bound parameter 
doesn't exist or the value provided to the bound parameter isn't a scalar value, an `InvalidArgumentException` is thrown

### Results

#### Fetch Methods

These methods map closely to the pdo statement methods of the same name. The `fetch` method will keep on returning data until
it reaches the end of the returned data where it will return false. The `fetchAll` method will collect all the data and
return it as one big array. 

Both methods return each row as an associative array with the column name as the key and the values are mapped directly 
to PHP types.

```php
$db = new \DuckDB\DuckDB(__DIR__ . '/Star_Trek-Season_1.csv');
$res = $db->query('SELECT episode_num FROM "Star_Trek-Season_1" limit 10');

while ($row = $res->fetch()) {
    echo $row['episode_num'] . "\n" ;
}

$db = new \DuckDB\DuckDB(__DIR__ . '/Star_Trek-Season_1.csv');
$res = $db->query('SELECT episode_num FROM "Star_Trek-Season_1" limit 10')->fetchAll();

foreach ($res as $row) {
    echo $row['episode_num'] . "\n" ;
}
```

### Appender

This extension provides an interface to the Appender feature provided by duckdb. The duckdb Appender is best suited for 
fast data loading opposed to inserting multiple rows via an INSERT query.

You can create an appender via the `append` method:

```php
$db = new \DuckDB\DuckDB();
$appender = $db->append('table');
```

You can then insert a row `appendRow` method:

```php
$appender->appendRow([1, 'Duck']);

$db->query('SELECT * FROM people')->print();

for ($i = 2; $i <= 10; $i++) {
    $appender->appendRow([$i, 'Duck' . $i]);
}
```

Rows aren't automatically appended to the table. You can explicitly flush the rows to the table via the `flush` method,
otherwise the rows will either be flushed when an internal buffer is filled up or when the appender is cleaned up via 
the garbage collector.

If you want to clear the appended rows, you can call the `clear` method.

> [!CAUTION]
> Care should be taken when inserting rows into the appender. Should an exception be thrown because of type mismatches,
> the appender can be left in an incomplete state. In such a situation, it's best that you call the clear method
