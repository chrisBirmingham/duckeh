#pragma once

#include "duckdb_structs.h"

void duckval_to_zval(duckdb_vector_t *vector_t, idx_t row_index, zval *data);

void duckdb_init_temp_vector(duckdb_vector_t *vector_t, duckdb_vector vector);

static inline void duckdb_destroy_temp_vector(duckdb_vector_t *vector_t)
{
  duckdb_destroy_logical_type(&vector_t->logical_type);
}

duckdb_value zval_to_duckval(zval *value);
