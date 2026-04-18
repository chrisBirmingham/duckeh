/* duckdb extension for PHP (c) 2025 Daniel Hernández-Marín */

#pragma once

extern zend_module_entry duckdb_module_entry;
# define phpext_duckdb_ptr &duckdb_module_entry

# define PHP_DUCKDB_VERSION "0.6.2"

# if defined(ZTS) && defined(COMPILE_DL_DUCKDB)
ZEND_TSRMLS_CACHE_EXTERN()
# endif
