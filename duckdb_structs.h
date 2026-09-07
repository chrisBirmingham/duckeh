#pragma once

#include "php.h"
#include <duckdb.h>
#include <zend_types.h>

/* Structs */
typedef struct {
  duckdb_database database;
  duckdb_connection connection;
  zend_object std;
} duckdb_t;

typedef struct {
  duckdb_prepared_statement stmt;
  zend_object std;
} duckdb_prepared_statement_t;

typedef struct {
  duckdb_appender appender;
  int64_t count;
  zend_object std;
} duckdb_appender_t;

typedef struct {
  duckdb_data_chunk current_chunk;
  idx_t current_row;
  duckdb_result result;
  zend_object std;
} duckdb_result_t;

typedef struct {
  zend_object std;
} duckdb_default_value_t;

/* Type transformers */
static inline duckdb_t *duckdb_t_from_obj(zend_object *obj)
{
  return (duckdb_t *)((char *)(obj)-offsetof(duckdb_t, std));
}

static inline duckdb_prepared_statement_t *prepared_statement_t_from_obj(zend_object *obj)
{
  return (duckdb_prepared_statement_t *)((char *)(obj)-offsetof(duckdb_prepared_statement_t, std));
}

static inline duckdb_appender_t *appender_t_from_obj(zend_object *obj)
{
  return (duckdb_appender_t *)((char *)(obj)-offsetof(duckdb_appender_t, std));
}

static inline duckdb_result_t *duckdb_result_t_from_obj(zend_object *obj)
{
  return (duckdb_result_t *)((char *)(obj)-offsetof(duckdb_result_t, std));
}

static inline duckdb_default_value_t *duckdb_default_value_t_from_obj(zend_object *obj)
{
  return (duckdb_default_value_t *)((char *)(obj)-offsetof(duckdb_default_value_t, std));
}

#define Z_DUCKDB_P(zv) duckdb_t_from_obj(Z_OBJ_P(zv))
#define Z_PREPARED_STATEMENT_P(zv) prepared_statement_t_from_obj(Z_OBJ_P(zv))
#define Z_APPENDER_P(zv) appender_t_from_obj(Z_OBJ_P(zv))
#define Z_DUCKDB_RESULT_P(zv) duckdb_result_t_from_obj(Z_OBJ_P(zv))
#define Z_DUCKDB_DEFAULT_VALUE_P(zv) duckdb_default_value_t_from_obj(Z_OBJ_P(zv))

extern zend_class_entry *duckdb_class_entry;
extern zend_class_entry *duckdb_prepared_statement_class_entry;
extern zend_class_entry *duckdb_appender_class_entry;
extern zend_class_entry *duckdb_result_class_entry;
extern zend_class_entry *duckdb_default_value_class_entry;
extern zend_class_entry *duckdb_exception_class_entry;
extern zend_class_entry *duckdb_connection_exception_class_entry;
extern zend_class_entry *duckdb_query_exception_class_entry;
extern zend_class_entry *duckdb_append_exception_class_entry;
