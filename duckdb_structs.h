#pragma once

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
  zend_object std;
} duckdb_appender_t;

typedef struct {
  duckdb_data_chunk current_chunk;
  idx_t current_row;
  duckdb_result result;
  zend_object std;
} duckdb_result_t;

typedef struct {
  duckdb_timestamp timestamp;
  zend_object std;
} duckdb_timestamp_t;

typedef struct {
  duckdb_date date;
  zend_object std;
} duckdb_date_t;

typedef struct {
  bool tz;
  union {
    duckdb_time time;
    duckdb_time_tz time_tz;
  };
  zend_object std;
} duckdb_time_t;

/* Type transformers */
static inline duckdb_t *duckdb_t_from_obj(zend_object *obj)
{
  return (duckdb_t *)((char *)(obj)-XtOffsetOf(duckdb_t, std));
}

static inline duckdb_prepared_statement_t *prepared_statement_t_from_obj(zend_object *obj)
{
  return (duckdb_prepared_statement_t *)((char *)(obj)-XtOffsetOf(duckdb_prepared_statement_t, std));
}

static inline duckdb_appender_t *appender_t_from_obj(zend_object *obj)
{
  return (duckdb_appender_t *)((char *)(obj)-XtOffsetOf(duckdb_appender_t, std));
}

static inline duckdb_result_t *duckdb_result_t_from_obj(zend_object *obj)
{
  return (duckdb_result_t *)((char *)(obj)-XtOffsetOf(duckdb_result_t, std));
}

static inline duckdb_timestamp_t *duckdb_timestamp_t_from_obj(zend_object *obj)
{
  return (duckdb_timestamp_t *)((char *)(obj)-XtOffsetOf(duckdb_timestamp_t, std));
}

static inline duckdb_date_t *duckdb_date_t_from_obj(zend_object *obj)
{
  return (duckdb_date_t *)((char *)(obj)-XtOffsetOf(duckdb_date_t, std));
}

static inline duckdb_time_t *duckdb_time_t_from_obj(zend_object *obj)
{
  return (duckdb_time_t *)((char *)(obj)-XtOffsetOf(duckdb_time_t, std));
}

#define Z_DUCKDB_P(zv) duckdb_t_from_obj(Z_OBJ_P(zv))
#define Z_PREPARED_STATEMENT_P(zv) prepared_statement_t_from_obj(Z_OBJ_P(zv))
#define Z_APPENDER_P(zv) appender_t_from_obj(Z_OBJ_P(zv))
#define Z_DUCKDB_RESULT_P(zv) duckdb_result_t_from_obj(Z_OBJ_P(zv))
#define Z_DUCKDB_TIMESTAMP_P(zv) duckdb_timestamp_t_from_obj(Z_OBJ_P(zv))
#define Z_DUCKDB_DATE_P(zv) duckdb_date_t_from_obj(Z_OBJ_P(zv))
#define Z_DUCKDB_TIME_P(zv) duckdb_time_t_from_obj(Z_OBJ_P(zv))

extern zend_class_entry *duckdb_class_entry;
extern zend_class_entry *duckdb_prepared_statement_class_entry;
extern zend_class_entry *duckdb_appender_class_entry;
extern zend_class_entry *duckdb_result_class_entry;
extern zend_class_entry *duckdb_timestamp_class_entry;
extern zend_class_entry *duckdb_date_class_entry;
extern zend_class_entry *duckdb_time_class_entry;
extern zend_class_entry *duckdb_exception_class_entry;
extern zend_class_entry *duckdb_connection_exception_class_entry;
extern zend_class_entry *duckdb_query_exception_class_entry;
extern zend_class_entry *duckdb_append_exception_class_entry;
