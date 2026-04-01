# 0.3.0

* Replaced deprecated `duckdb_row_count` with `duckdb_rows_changed`. `rowCount` should now be closer to PDO's `rowCount` method

# 0.2.0

* Removed the sql method
* Move to only stored a timestamp struct in the time class
* Fixed memory leaks in `__toString` methods for Timestamp and Date classes
* Raise valueException if the append array is empty
* Deleted old ci, will update later

# 0.1.0

* Initial version
