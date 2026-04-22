#pragma once

#include "php.h"
#include <duckdb.h>

void duckdb_value_to_zval_string(duckdb_value value, zval *data);

void duckval_to_zval(duckdb_vector vector, duckdb_type type, idx_t row_index, zval *data);

duckdb_value zval_to_duckval(zval *value);
