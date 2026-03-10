## DuckDB API for PHP

A wrapper over DuckDB C API

> [!CAUTION]
> This project is in an early development stage and there are no plans to make it stable/production ready for now. Nevertheless, any contribution will be welcome. Check also [satur-io/duckdb-php](https://github.com/satur-io/duckdb-php)

## Prerequisites

Install the DuckDB C library (`libduckdb`) into a standard system path before using PIE.

<details>
<summary>Linux</summary>

Official downloads: https://duckdb.org/install/?platform=linux&environment=c

#### x86_64 (amd64)

```sh
tmp="$(mktemp -d)"
curl -L "https://install.duckdb.org/v1.4.4/libduckdb-linux-amd64.zip" -o "$tmp/libduckdb.zip"
unzip -q "$tmp/libduckdb.zip" -d "$tmp"
sudo mkdir -p /usr/local/include /usr/local/lib
sudo install -m 644 "$tmp/duckdb.h" /usr/local/include/duckdb.h
sudo install -m 755 "$tmp/libduckdb.so" /usr/local/lib/libduckdb.so
sudo ldconfig
```

#### arm64 (aarch64)

```sh
tmp="$(mktemp -d)"
curl -L "https://install.duckdb.org/v1.4.4/libduckdb-linux-arm64.zip" -o "$tmp/libduckdb.zip"
unzip -q "$tmp/libduckdb.zip" -d "$tmp"
sudo mkdir -p /usr/local/include /usr/local/lib
sudo install -m 644 "$tmp/duckdb.h" /usr/local/include/duckdb.h
sudo install -m 755 "$tmp/libduckdb.so" /usr/local/lib/libduckdb.so
sudo ldconfig
```

</details>

<details>
<summary>macOS</summary>

Official downloads: https://duckdb.org/install/?platform=macos&environment=c

```sh
tmp="$(mktemp -d)"
curl -L "https://install.duckdb.org/v1.4.4/libduckdb-osx-universal.zip" -o "$tmp/libduckdb.zip"
unzip -q "$tmp/libduckdb.zip" -d "$tmp"
sudo mkdir -p /usr/local/include /usr/local/lib
sudo install -m 644 "$tmp/duckdb.h" /usr/local/include/duckdb.h
sudo install -m 755 "$tmp/libduckdb.dylib" /usr/local/lib/libduckdb.dylib
```

</details>

<details>
<summary>Windows</summary>

Official downloads: https://duckdb.org/install/?platform=windows&environment=c

Open PowerShell as Administrator.

#### x86_64 (amd64)

```powershell
$vc = "$env:TEMP\vc_redist.x64.exe"
Invoke-WebRequest "https://aka.ms/vc14/vc_redist.x64.exe" -OutFile $vc
Start-Process -Wait -FilePath $vc -ArgumentList "/install","/passive","/norestart"

$version = "v1.4.4"
$zip = "libduckdb-windows-amd64.zip"
$dest = "$env:TEMP\duckdb"
New-Item -ItemType Directory -Force $dest | Out-Null
Invoke-WebRequest "https://install.duckdb.org/$version/$zip" -OutFile "$dest\libduckdb.zip"
Expand-Archive "$dest\libduckdb.zip" -DestinationPath $dest -Force
$target = "C:\Program Files\DuckDB"
New-Item -ItemType Directory -Force $target | Out-Null
Copy-Item "$dest\*" $target -Recurse -Force
$system = "C:\Windows"
Copy-Item "$dest\duckdb.dll" "$system\duckdb.dll" -Force
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$target", "User")
$env:Path += ";$target"
```

#### arm64

```powershell
$vc = "$env:TEMP\vc_redist.arm64.exe"
Invoke-WebRequest "https://aka.ms/vc14/vc_redist.arm64.exe" -OutFile $vc
Start-Process -Wait -FilePath $vc -ArgumentList "/install","/passive","/norestart"

$version = "v1.4.4"
$zip = "libduckdb-windows-arm64.zip"
$dest = "$env:TEMP\duckdb"
New-Item -ItemType Directory -Force $dest | Out-Null
Invoke-WebRequest "https://install.duckdb.org/$version/$zip" -OutFile "$dest\libduckdb.zip"
Expand-Archive "$dest\libduckdb.zip" -DestinationPath $dest -Force
$target = "C:\Program Files\DuckDB"
New-Item -ItemType Directory -Force $target | Out-Null
Copy-Item "$dest\*" $target -Recurse -Force
$system = "C:\Windows"
Copy-Item "$dest\duckdb.dll" "$system\duckdb.dll" -Force
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$target", "User")
$env:Path += ";$target"
```

</details>

## Installation (PIE)

PIE installs PHP extensions published on Packagist. Once this package is published, install it with:

```sh
pie install saturio/duckdb
```

If PIE cannot enable the extension automatically, add this to your INI:

```ini
extension=duckdb
```

## Library in a non-standard path (advanced)

If you installed `libduckdb` outside the standard system paths, pass the prefix that contains `include/duckdb.h` and `lib/libduckdb.*`:

```sh
pie install saturio/duckdb --with-duckdb-dir=/path/to/duckdb
```

You can see available configure options with:

```sh
pie info saturio/duckdb
```

PIE lists the available configure options for an extension in `pie info`. Use those options with `pie install`.

If `pie` reports that `--with-duckdb-dir` does not exist, install a version that includes this option and confirm it appears in `pie info` before retrying.

As a fallback, you can also pass include and library paths via environment variables:

```sh
CPPFLAGS="-I/path/to/duckdb/include" LDFLAGS="-L/path/to/duckdb/lib" pie install saturio/duckdb
```

## Usage

### Creation

A new in-memory database can be created like so:

```php
$db = new \DuckDB\DuckDB();
```

You can attach to a store by specifying a path to the DuckDB class.

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
$stmt = $db->prepare('SELECT * FROM duck where quack = $1');

# Bind a variable to the parameter
$stmt->bindParam(1, "honk");

# Execute the prepared statement
$res = $stmt->execute();
```

The `execute` method returns a `Result` class or throws a `QueryException` if the query fails

### Results

The result class supports two forms for getting the queried data, the higher level fetch method and lower level chunk method

#### Fetch methods

These methods closely map to the pdo statement methods of the same name. The `fetch` method will keep on returning data until 
it reaches the end of the returned data where it will return null. The `fetchAll` method will collect all the data and
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

### Chunk method

This extension also supports duckdb's lower level chunk and vector datatypes. You can select a chunk via the `fetchChunk`
method. This method will keep on returning chunks until all data is exhausted. You then process each chunk like so: 

```php
$duckDB = new \DuckDB\DuckDB();

$result = $duckDB->query("SELECT 'quack' as mycolumn1, 'quick' as mycolumn2;");

# Get how many columns the response has
$columns = $result->columnCount();

# Keep on getting data until we've exhausted the input
while ($dataChunk = $result->fetchChunk()) {
    # Get how many rows are inside the chunk
    $rows = $dataChunk->getSize();
    
    # Loop over all the rows in the chunk
    for ($j = 0; $j < $rows; $j++) {
        
        for ($i = 0; $i < $columns; $i++) {
            # Get the vector for the column
            $vector = $dataChunk->getVector($i);
            
            # Get the value of the column for row $j
            $data = $vector->getData($j);
            var_dump($data);
        }
    }
}
```
