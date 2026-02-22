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
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$target", "User")
$env:Path += ";$target"
```

</details>

## Installation (PIE)

PIE installs PHP extensions published on Packagist. Once this package is published, install it with:

```sh
pie install saturio/duckdb-ext
```

If PIE cannot enable the extension automatically, add this to your INI:

```ini
extension=duckdb
```

## Library in a non-standard path (advanced)

If you installed `libduckdb` outside the standard system paths, pass the prefix that contains `include/duckdb.h` and `lib/libduckdb.*`:

```sh
pie install saturio/duckdb-ext --with-duckdb-dir=/path/to/duckdb
```

You can see available configure options with:

```sh
pie info saturio/duckdb-ext
```
