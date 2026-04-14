# 0.6.0

* Remove lower level chunk and vector classes
* Big refactoring of codebase
  * Split values funcs into own file
  * Removed vector_t type and associated funcs
  * Moved common timestamp code into func

# 0.5.3

* Fix DuckDB::append not accepting correct number of arguments

# 0.5.2

* Replace sprintf with `zend_strpprintf` in bigint conversion path.

# 0.5.1

* Use `INTERNAL_FUNCTION_PARAM_PASSTHRU` to reduce code duplication
* Additional code cleanup

# 0.5.0

* Remove the print method from the Result object

# 0.4.0

* Disallow creating classes via their constructors
* Deprecate the print method

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
