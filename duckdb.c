/* duckdb extension for PHP (c) 2025 Daniel Hernández-Marín */

#define PHP_DUCKDB_POSITIVE_INFINITY 1
#define PHP_DUCKDB_NEGATIVE_INFINITY -1
#define PHP_DUCKDB_FINITE 0

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/standard/info.h"
#include "php_duckdb.h"
#ifndef ZEND_ACC_NOT_SERIALIZABLE
#define ZEND_ACC_NOT_SERIALIZABLE 0
#endif
#include "duckdb_arginfo.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "duckdb_structs.h"
#include "duckdb_values.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE()  \
  ZEND_PARSE_PARAMETERS_START(0, 0) \
  ZEND_PARSE_PARAMETERS_END()
#endif

static zend_object_handlers duckdb_object_handlers;
static zend_object_handlers prepared_statement_object_handlers;
static zend_object_handlers append_statement_object_handlers;
static zend_object_handlers result_object_handlers;
static zend_object_handlers timestamp_object_handlers;
static zend_object_handlers date_object_handlers;
static zend_object_handlers time_object_handlers;

zend_class_entry *duckdb_class_entry = NULL;
zend_class_entry *duckdb_prepared_statement_class_entry = NULL;
zend_class_entry *duckdb_appender_class_entry = NULL;
zend_class_entry *duckdb_result_class_entry = NULL;
zend_class_entry *duckdb_timestamp_class_entry = NULL;
zend_class_entry *duckdb_date_class_entry = NULL;
zend_class_entry *duckdb_time_class_entry = NULL;
zend_class_entry *duckdb_exception_class_entry = NULL;
zend_class_entry *duckdb_connection_exception_class_entry = NULL;
zend_class_entry *duckdb_query_exception_class_entry = NULL;
zend_class_entry *duckdb_append_exception_class_entry = NULL;

static inline void free_config(duckdb_config *config)
{
  if (config) {
    duckdb_destroy_config(config);
  }
}

static zend_result create_config(duckdb_config *config, HashTable *options)
{
  zend_string *key = NULL;
  zval *value = NULL;

  if (duckdb_create_config(config) == DuckDBError) {
    return FAILURE;
  }

  ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(options, key, value) {
    if (Z_TYPE_P(value) != IS_STRING) {
      zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid value for config option '%s'. Value must be a string", ZSTR_VAL(key));
      return FAILURE;
    }

    if (duckdb_set_config(*config, ZSTR_VAL(key), Z_STRVAL_P(value)) == DuckDBError) {
      zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Failed to set config option '%s'", ZSTR_VAL(key));
      return FAILURE;
    }
  } ZEND_HASH_FOREACH_END();

  return SUCCESS;
}

static zend_result connect_to_db(duckdb_database *database, duckdb_connection *connection, const char* path, HashTable *options)
{
  duckdb_config config = NULL;
  char *err = NULL;

  if (options && create_config(&config, options) == FAILURE) {
    free_config(&config);
    return FAILURE;
  }

  if (duckdb_open_ext(path, database, config, &err) == DuckDBError) {
    zend_throw_exception(duckdb_connection_exception_class_entry, err, 0);
    free_config(&config);
    duckdb_free(err);
    return FAILURE;
  }

  if (duckdb_connect(*database, connection) == DuckDBError) {
    duckdb_close(database);
    free_config(&config);
    zend_throw_exception(duckdb_connection_exception_class_entry, "Failed to connect to initialised duckdb database", 0);
    return FAILURE;
  }

  return SUCCESS;
}

static zend_object *duckdb_new(zend_class_entry *ce)
{
  duckdb_t *duckdb = zend_object_alloc(sizeof(duckdb_t), ce);
  zend_object_std_init(&duckdb->std, ce);
  object_properties_init(&duckdb->std, ce);
  duckdb->std.handlers = &duckdb_object_handlers;
  return &duckdb->std;
}

static void duckdb_free_obj(zend_object *obj)
{
  duckdb_t *duckdb = duckdb_t_from_obj(obj);
  duckdb_disconnect(&duckdb->connection);
  duckdb_close(&duckdb->database);
  zend_object_std_dtor(&duckdb->std);
}

PHP_METHOD(DuckDB_DuckDB, __construct)
{
  zval *object = ZEND_THIS;
  duckdb_t *duckdb_t;
  char *path = NULL;
  size_t path_len = 0;
  HashTable *options = NULL;
  duckdb_database database;
  duckdb_connection connection;

  ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_STRING_OR_NULL(path, path_len)
    Z_PARAM_ARRAY_HT_OR_NULL(options)
  ZEND_PARSE_PARAMETERS_END();

  if (connect_to_db(&database, &connection, path, options) == FAILURE) {
    RETURN_THROWS();
  }

  duckdb_t = Z_DUCKDB_P(object);
  duckdb_t->database = database;
  duckdb_t->connection = connection;
}

static void duckdb_run_query(INTERNAL_FUNCTION_PARAMETERS, bool is_prepared)
{
  zval *object = ZEND_THIS;
  duckdb_t *duckdb_t;
  duckdb_state state;
  duckdb_prepared_statement stmt;
  duckdb_result res;
  char *query = NULL;
  size_t query_len = 0;

  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(query, query_len)
  ZEND_PARSE_PARAMETERS_END();

  duckdb_t = Z_DUCKDB_P(object);

  state = is_prepared
    ? duckdb_prepare(duckdb_t->connection, query, &stmt)
    : duckdb_query(duckdb_t->connection, query, &res);

  if (state == DuckDBError) {
    if (is_prepared) {
      zend_throw_exception(duckdb_query_exception_class_entry, duckdb_prepare_error(stmt), 0);
      duckdb_destroy_prepare(&stmt);
    } else {
      zend_throw_exception(duckdb_query_exception_class_entry, duckdb_result_error(&res), 0);
      duckdb_destroy_result(&res);
    }

    RETURN_THROWS();
  }

  if (is_prepared) {
    object_init_ex(return_value, duckdb_prepared_statement_class_entry);
    duckdb_prepared_statement_t *prepared_statement_t = Z_PREPARED_STATEMENT_P(return_value);
    prepared_statement_t->stmt = stmt;
  } else {
    object_init_ex(return_value, duckdb_result_class_entry);
    duckdb_result_t *result_t = Z_DUCKDB_RESULT_P(return_value);
    result_t->result = res;
  }
}

PHP_METHOD(DuckDB_DuckDB, query)
{
  duckdb_run_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DuckDB_DuckDB, prepare)
{
  duckdb_run_query(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

static void appender_error(duckdb_appender appender)
{
  duckdb_error_data err = duckdb_appender_error_data(appender);
  zend_throw_exception(duckdb_append_exception_class_entry, duckdb_error_data_message(err), 0);
  duckdb_destroy_error_data(&err);
}

PHP_METHOD(DuckDB_DuckDB, append)
{
  zval *object = ZEND_THIS;
  duckdb_t *duckdb_t;
  char *table = NULL;
  size_t table_len = 0;
  char *schema = NULL;
  size_t schema_len = 0;
  char *catalogue = NULL;
  size_t catalogue_len = 0;
  duckdb_appender appender;

  ZEND_PARSE_PARAMETERS_START(1, 3)
    Z_PARAM_STRING(table, table_len)
    Z_PARAM_OPTIONAL
    Z_PARAM_STRING_OR_NULL(schema, schema_len)
    Z_PARAM_STRING_OR_NULL(catalogue, catalogue_len)
  ZEND_PARSE_PARAMETERS_END();

  duckdb_t = Z_DUCKDB_P(object);

  if (duckdb_appender_create_ext(duckdb_t->connection, catalogue, schema, table, &appender) == DuckDBError) {
    appender_error(appender);
    duckdb_appender_destroy(&appender);
    RETURN_THROWS();
  }

  object_init_ex(return_value, duckdb_appender_class_entry);
  duckdb_appender_t *appender_t = Z_APPENDER_P(return_value);
  appender_t->appender = appender;
}

static zend_object *duckdb_prepared_statement_new(zend_class_entry *ce)
{
  duckdb_prepared_statement_t *prepared_statement = zend_object_alloc(sizeof(duckdb_prepared_statement_t), ce);
  zend_object_std_init(&prepared_statement->std, ce);
  object_properties_init(&prepared_statement->std, ce);
  prepared_statement->std.handlers = &prepared_statement_object_handlers;
  return &prepared_statement->std;
}

static void duckdb_prepared_statement_free_obj(zend_object *obj)
{
  duckdb_prepared_statement_t *prepared_statement = prepared_statement_t_from_obj(obj);
  duckdb_destroy_prepare(&prepared_statement->stmt);
  zend_object_std_dtor(&prepared_statement->std);
}

static zend_function *duckdb_prepared_statement_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Prepared Statement. use DuckDB::prepare");
  return NULL;
}

static idx_t bind_param_string(duckdb_prepared_statement *stmt, const char *param, zval *value, duckdb_value *ret)
{
  idx_t index;

  if (duckdb_bind_parameter_index(*stmt, &index, param) == DuckDBError) {
    zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Unknown named parameter '%s'", param);
    return 0;
  }

  if ((*ret = zval_to_duckval(value)) == NULL) {
    zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid value provided for named parameter '%s'. Value must be a scalar type", param);
    return 0;
  }

  return index;
}

static idx_t bind_param_numeric(duckdb_prepared_statement *stmt, zend_long index, zval *value, duckdb_value *ret)
{
  if (index <= 0 || index > duckdb_nparams(*stmt)) {
    zend_throw_exception_ex(spl_ce_OutOfBoundsException, 0, "Parameter index '" ZEND_LONG_FMT "' is out of bounds", index);
    return 0;
  }

  if ((*ret = zval_to_duckval(value)) == NULL) {
    zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid value provided for parameter index '" ZEND_LONG_FMT "'. Value must be a scalar type", index);
    return 0;
  }

  return index;
}

static zend_result bind_param(duckdb_prepared_statement *stmt, zend_string *str_param, zend_long long_param, zval *value)
{
  duckdb_value val = NULL;
  duckdb_state state;
  idx_t idx = (str_param)
    ? bind_param_string(stmt, ZSTR_VAL(str_param), value, &val)
    : bind_param_numeric(stmt, long_param, value, &val);

  if (idx == 0) {
    return FAILURE;
  }

  state = duckdb_bind_value(*stmt, idx, val);
  duckdb_destroy_value(&val);

  if (state == DuckDBError) {
    zend_throw_exception(duckdb_query_exception_class_entry, duckdb_prepare_error(*stmt), 0);
    return FAILURE;
  }

  return SUCCESS;
}

PHP_METHOD(DuckDB_PreparedStatement, bindParam)
{
  zval *object = ZEND_THIS;
  duckdb_prepared_statement_t *prepared_statement_t;
  zend_string *str_param = NULL;
  zend_long long_param = 0;
  zval *value = NULL;

  ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STR_OR_LONG(str_param, long_param)
    Z_PARAM_ZVAL(value)
  ZEND_PARSE_PARAMETERS_END();

  prepared_statement_t = Z_PREPARED_STATEMENT_P(object);

  if (bind_param(&prepared_statement_t->stmt, str_param, long_param, value) == FAILURE) {
    RETURN_THROWS();
  }
}

PHP_METHOD(DuckDB_PreparedStatement, execute)
{
  zval *object = ZEND_THIS;
  duckdb_prepared_statement_t *prepared_statement_t;
  duckdb_result_t *result_t;
  duckdb_result res;
  zval *params = NULL;
  zend_ulong idx;
  zend_string *key = NULL;
  zval *value = NULL;

  ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_ARRAY_OR_NULL(params)
  ZEND_PARSE_PARAMETERS_END();

  prepared_statement_t = Z_PREPARED_STATEMENT_P(object);

  if (params) {
    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR(*params), idx, key, value) {
      if (bind_param(&prepared_statement_t->stmt, key, idx, value) == FAILURE) {
        RETURN_THROWS();
      }
    } ZEND_HASH_FOREACH_END();
  }

  if (duckdb_execute_prepared(prepared_statement_t->stmt, &res) == DuckDBError) {
    zend_throw_exception(duckdb_query_exception_class_entry, duckdb_result_error(&res), 0);
    duckdb_destroy_result(&res);
    RETURN_THROWS();
  }

  object_init_ex(return_value, duckdb_result_class_entry);
  result_t = Z_DUCKDB_RESULT_P(return_value);
  result_t->result = res;
}

static zend_object *duckdb_appender_new(zend_class_entry *ce)
{
  duckdb_appender_t *append_statement = zend_object_alloc(sizeof(duckdb_appender_t), ce);
  zend_object_std_init(&append_statement->std, ce);
  object_properties_init(&append_statement->std, ce);
  append_statement->std.handlers = &append_statement_object_handlers;
  return &append_statement->std;
}

static void duckdb_appender_free_obj(zend_object *obj)
{
  duckdb_appender_t *appender = appender_t_from_obj(obj);
  duckdb_appender_destroy(&appender->appender);
  zend_object_std_dtor(&appender->std);
}

static zend_function *duckdb_appender_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate an Appender, use DuckDB::append instead");
  return NULL;
}

static zend_result append_row(duckdb_appender appender, zend_array *row)
{
  zval *value = NULL;
  zend_ulong len = zend_array_count(row);
  zend_ulong expected = duckdb_appender_column_count(appender);

  if (len != expected) {
    zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Row does not match expected column count. Expected '" ZEND_LONG_FMT "' got '" ZEND_LONG_FMT"'", expected, len);
    return FAILURE;
  }

  ZEND_HASH_FOREACH_VAL(row, value) {
    duckdb_value val = zval_to_duckval(value);

    if (val == NULL) {
      zend_throw_exception_ex(spl_ce_InvalidArgumentException, 0, "Invalid value in row. Columns values must be scalar type");
      return FAILURE;
    }

    if (duckdb_append_value(appender, val) == DuckDBError) {
      appender_error(appender);
      return FAILURE;
    }

    duckdb_destroy_value(&val);
  } ZEND_HASH_FOREACH_END();

  if (duckdb_appender_end_row(appender) == DuckDBError) {
    appender_error(appender);
    return FAILURE;
  }

  return SUCCESS;
}

PHP_METHOD(DuckDB_Appender, appendRow)
{
  zval *object = ZEND_THIS;
  duckdb_appender_t *append_t;
  zval *arr = NULL;
  zend_array *row = NULL;

  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ARRAY(arr)
  ZEND_PARSE_PARAMETERS_END();

  append_t = Z_APPENDER_P(object);
  row = Z_ARR(*arr);

  if (zend_array_count(row) == 0) {
    zend_argument_value_error(1, "must not be empty");
    RETURN_THROWS();
  }

  if (append_row(append_t->appender, row) == FAILURE) {
    RETURN_THROWS();
  }
}

PHP_METHOD(DuckDB_Appender, flush)
{
  zval *object = ZEND_THIS;
  duckdb_appender_t *append_t;

  ZEND_PARSE_PARAMETERS_NONE();

  append_t = Z_APPENDER_P(object);

  if (duckdb_appender_flush(append_t->appender) == DuckDBError) {
    appender_error(append_t->appender);
    RETURN_THROWS();
  }
}

PHP_METHOD(DuckDB_Appender, clear)
{
  zval *object = ZEND_THIS;
  duckdb_appender_t *append_t;

  ZEND_PARSE_PARAMETERS_NONE();

  append_t = Z_APPENDER_P(object);

  if (duckdb_appender_clear(append_t->appender) == DuckDBError) {
    appender_error(append_t->appender);
    RETURN_THROWS();
  }
}

static zend_object *duckdb_result_new(zend_class_entry *ce)
{
  duckdb_result_t *result = zend_object_alloc(sizeof(duckdb_result_t), ce);
  zend_object_std_init(&result->std, ce);
  object_properties_init(&result->std, ce);
  result->std.handlers = &result_object_handlers;
  result->current_chunk = NULL;
  return &result->std;
}

static void duckdb_result_free_obj(zend_object *obj)
{
  duckdb_result_t *result_t = duckdb_result_t_from_obj(obj);

  if (result_t->current_chunk != NULL) {
    duckdb_destroy_data_chunk(&result_t->current_chunk);
  }

  duckdb_destroy_result(&result_t->result);
  zend_object_std_dtor(&result_t->std);
}

static zend_function *duckdb_result_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Result, use DuckDB::query or PreparedStatement::execute");
  return NULL;
}

static void result_get_prop(INTERNAL_FUNCTION_PARAMETERS, bool get_rows)
{
  zval *object = ZEND_THIS;
  duckdb_result_t *result_t;
  zend_long prop = 0;

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);

  prop = get_rows ? duckdb_rows_changed(&result_t->result) : duckdb_column_count(&result_t->result);
  RETURN_LONG(prop);
}

PHP_METHOD(DuckDB_Result, rowCount)
{
  result_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(DuckDB_Result, columnCount)
{
  result_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

static void fetch_row(zval *arr, duckdb_result *res, duckdb_data_chunk chunk, idx_t column_count, idx_t row)
{
  for (idx_t c = 0; c < column_count; c++) {
    zval value;
    duckval_to_zval(duckdb_data_chunk_get_vector(chunk, c), duckdb_column_type(res, c), row, &value);
    add_assoc_zval(arr, duckdb_column_name(res, c), &value);
  }
}

PHP_METHOD(DuckDB_Result, fetch)
{
  zval *object = ZEND_THIS;
  duckdb_result_t *result_t;
  duckdb_data_chunk chunk;

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);

  if (result_t->current_chunk == NULL) {
    result_t->current_chunk = duckdb_fetch_chunk(result_t->result);

    if (result_t->current_chunk == NULL) {
      RETURN_FALSE;
    }

    result_t->current_row = 0;
  }

  chunk = result_t->current_chunk;
  idx_t column_count = duckdb_column_count(&result_t->result);
  array_init_size(return_value, column_count);
  fetch_row(return_value, &result_t->result, chunk, column_count, result_t->current_row);

  if (++result_t->current_row == duckdb_data_chunk_get_size(chunk)) {
    duckdb_destroy_data_chunk(&chunk);
    result_t->current_chunk = NULL;
  }
}

PHP_METHOD(DuckDB_Result, fetchAll)
{
  zval *object = ZEND_THIS;
  duckdb_result_t *result_t;
  duckdb_data_chunk chunk;

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);
  idx_t column_count = duckdb_column_count(&result_t->result);
  array_init(return_value);

  while ((chunk = duckdb_fetch_chunk(result_t->result)) != NULL) {
    idx_t chunk_size = duckdb_data_chunk_get_size(chunk);

    for (idx_t r = 0; r < chunk_size; r++) {
      zval arr;
      array_init_size(&arr, column_count);
      fetch_row(&arr, &result_t->result, chunk, column_count, r);
      add_next_index_zval(return_value, &arr);
    }

    duckdb_destroy_data_chunk(&chunk);
  }
}

static zend_object *duckdb_timestamp_new(zend_class_entry *ce)
{
  duckdb_timestamp_t *timestamp = zend_object_alloc(sizeof(duckdb_timestamp_t), ce);
  zend_object_std_init(&timestamp->std, ce);
  object_properties_init(&timestamp->std, ce);
  timestamp->std.handlers = &timestamp_object_handlers;
  return &timestamp->std;
}

static inline void duckdb_timestamp_free_obj(zend_object *obj)
{
  duckdb_timestamp_t *timestamp = duckdb_timestamp_t_from_obj(obj);
  zend_object_std_dtor(&timestamp->std);
}

static zend_function *duckdb_timestamp_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Timestamp object");
  return NULL;
}

PHP_METHOD(DuckDB_Value_Timestamp, infinity)
{
  zval *object = ZEND_THIS;
  duckdb_timestamp_t *timestamp_t;

  ZEND_PARSE_PARAMETERS_NONE();

  timestamp_t = Z_DUCKDB_TIMESTAMP_P(object);

  if (duckdb_is_finite_timestamp(timestamp_t->timestamp)) {
    RETURN_LONG(PHP_DUCKDB_FINITE);
  }

  if (timestamp_t->timestamp.micros > 0) {
    RETURN_LONG(PHP_DUCKDB_POSITIVE_INFINITY);
  }

  RETURN_LONG(PHP_DUCKDB_NEGATIVE_INFINITY);
}

static void timestamp_get_sub(INTERNAL_FUNCTION_PARAMETERS, bool get_time)
{
  zval *object = ZEND_THIS;
  duckdb_timestamp_t *timestamp_t;
  duckdb_timestamp_struct timestamp_struct;

  ZEND_PARSE_PARAMETERS_NONE();

  timestamp_t = Z_DUCKDB_TIMESTAMP_P(object);
  timestamp_struct = duckdb_from_timestamp(timestamp_t->timestamp);

  if (get_time) {
    object_init_ex(return_value, duckdb_time_class_entry);
    duckdb_time_t *time_t = Z_DUCKDB_TIME_P(return_value);
    time_t->time = timestamp_struct.time;
  } else {
    object_init_ex(return_value, duckdb_date_class_entry);
    duckdb_date_t *date_t = Z_DUCKDB_DATE_P(return_value);
    date_t->date = duckdb_to_date(timestamp_struct.date);
  }
}

PHP_METHOD(DuckDB_Value_Timestamp, getDate)
{
  timestamp_get_sub(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DuckDB_Value_Timestamp, getTime)
{
  timestamp_get_sub(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(DuckDB_Value_Timestamp, __toString)
{
  zval *object = ZEND_THIS;
  duckdb_timestamp_t *timestamp_t;
  duckdb_value time;

  ZEND_PARSE_PARAMETERS_NONE();

  timestamp_t = Z_DUCKDB_TIMESTAMP_P(object);
  time = duckdb_create_timestamp(timestamp_t->timestamp);
  RETURN_STRING(duckdb_get_varchar(time));
  duckdb_destroy_value(&time);
}

static zend_object *duckdb_date_new(zend_class_entry *ce)
{
  duckdb_date_t *date = zend_object_alloc(sizeof(duckdb_date_t), ce);
  zend_object_std_init(&date->std, ce);
  object_properties_init(&date->std, ce);
  date->std.handlers = &date_object_handlers;
  return &date->std;
}

static inline void duckdb_date_free_obj(zend_object *obj)
{
  duckdb_date_t *date = duckdb_date_t_from_obj(obj);
  zend_object_std_dtor(&date->std);
}

static zend_function *duckdb_date_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Date object");
  return NULL;
}

PHP_METHOD(DuckDB_Value_Date, infinity)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);

  if (duckdb_is_finite_date(date_t->date)) {
    RETURN_LONG(PHP_DUCKDB_FINITE);
  }

  if (date_t->date.days > 0) {
    RETURN_LONG(PHP_DUCKDB_POSITIVE_INFINITY);
  }

  RETURN_LONG(PHP_DUCKDB_NEGATIVE_INFINITY);
}

static void date_get_prop(INTERNAL_FUNCTION_PARAMETERS, char type)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;
  duckdb_date_struct date;
  long prop = 0;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);
  date = duckdb_from_date(date_t->date);

  switch (type) {
    case 'y':
      prop = date.year;
      break;
    case 'm':
      prop = date.month;
      break;
    case 'd':
      prop = date.day;
      break;
    case 'D':
      prop = date_t->date.days;
  }

  RETURN_LONG(prop);
}

PHP_METHOD(DuckDB_Value_Date, getYear)
{
  date_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'y');
}

PHP_METHOD(DuckDB_Value_Date, getMonth)
{
  date_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'm');
}

PHP_METHOD(DuckDB_Value_Date, getDay)
{
  date_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'd');
}

PHP_METHOD(DuckDB_Value_Date, getDays)
{
  date_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'D');
}

PHP_METHOD(DuckDB_Value_Date, __toString)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;
  duckdb_value date;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);
  date = duckdb_create_date(date_t->date);
  RETURN_STRING(duckdb_get_varchar(date));
  duckdb_destroy_value(&date);
}

static zend_object *duckdb_time_new(zend_class_entry *ce)
{
  duckdb_time_t *time = zend_object_alloc(sizeof(duckdb_time_t), ce);
  zend_object_std_init(&time->std, ce);
  object_properties_init(&time->std, ce);
  time->std.handlers = &time_object_handlers;
  return &time->std;
}

static inline void duckdb_time_free_obj(zend_object *obj)
{
  duckdb_time_t *time_t = duckdb_time_t_from_obj(obj);
  zend_object_std_dtor(&time_t->std);
}

static zend_function *duckdb_time_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Time object");
  return NULL;
}

static void time_get_prop(INTERNAL_FUNCTION_PARAMETERS, char type)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;
  zend_long prop = 0;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);

  switch (type) {
    case 'h':
      prop = time_t->time.hour;
      break;
    case 'm':
      prop = time_t->time.min;
      break;
    case 's':
      prop = time_t->time.sec;
      break;
    case 'i':
      prop = time_t->time.micros;
      break;
    case 'I':
      prop = duckdb_to_time(time_t->time).micros;
  }

  RETURN_LONG(prop);
}


PHP_METHOD(DuckDB_Value_Time, getHour)
{
  time_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'h');
}

PHP_METHOD(DuckDB_Value_Time, getMinutes)
{
  time_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'm');
}

PHP_METHOD(DuckDB_Value_Time, getSeconds)
{
  time_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 's');
}

PHP_METHOD(DuckDB_Value_Time, getMicroseconds)
{
  time_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'i');
}

PHP_METHOD(DuckDB_Value_Time, getTotalMicroseconds)
{
  time_get_prop(INTERNAL_FUNCTION_PARAM_PASSTHRU, 'I');
}

PHP_METHOD(DuckDB_Value_Time, __toString)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;
  duckdb_time_struct time;
  zend_string *str = NULL;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);
  time = time_t->time;

  str = zend_strpprintf(0, "%02d:%02d:%02d.%d", time.hour, time.min, time.sec, time.micros);
  RETURN_STR(str);
}

PHP_RINIT_FUNCTION(duckdb)
{
#if defined(ZTS) && defined(COMPILE_DL_DUCKDB)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif

  return SUCCESS;
}

PHP_MINIT_FUNCTION(duckdb)
{
  duckdb_exception_class_entry = register_class_DuckDB_DuckDBException(zend_ce_exception);
  duckdb_connection_exception_class_entry = register_class_DuckDB_ConnectionException(duckdb_exception_class_entry);
  duckdb_query_exception_class_entry = register_class_DuckDB_QueryException(duckdb_exception_class_entry);
  duckdb_append_exception_class_entry = register_class_DuckDB_AppendException(duckdb_exception_class_entry);

  memcpy(&duckdb_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&prepared_statement_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&append_statement_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&result_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&timestamp_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&date_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&time_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));

  duckdb_class_entry = register_class_DuckDB_DuckDB();
  duckdb_class_entry->create_object = duckdb_new;
  duckdb_object_handlers.offset = XtOffsetOf(duckdb_t, std);
  duckdb_object_handlers.free_obj = duckdb_free_obj;

  duckdb_prepared_statement_class_entry = register_class_DuckDB_PreparedStatement();
  duckdb_prepared_statement_class_entry->create_object = duckdb_prepared_statement_new;
  prepared_statement_object_handlers.offset = XtOffsetOf(duckdb_prepared_statement_t, std);
  prepared_statement_object_handlers.get_constructor = duckdb_prepared_statement_constructor;
  prepared_statement_object_handlers.free_obj = duckdb_prepared_statement_free_obj;

  duckdb_appender_class_entry = register_class_DuckDB_Appender();
  duckdb_appender_class_entry->create_object = duckdb_appender_new;
  append_statement_object_handlers.offset = XtOffsetOf(duckdb_appender_t, std);
  append_statement_object_handlers.get_constructor = duckdb_appender_constructor;
  append_statement_object_handlers.free_obj = duckdb_appender_free_obj;

  duckdb_result_class_entry = register_class_DuckDB_Result();
  duckdb_result_class_entry->create_object = duckdb_result_new;
  result_object_handlers.offset = XtOffsetOf(duckdb_result_t, std);
  result_object_handlers.get_constructor = duckdb_result_constructor;
  result_object_handlers.free_obj = duckdb_result_free_obj;

  duckdb_timestamp_class_entry = register_class_DuckDB_Value_Timestamp(zend_ce_stringable);
  duckdb_timestamp_class_entry->create_object = duckdb_timestamp_new;
  timestamp_object_handlers.offset = XtOffsetOf(duckdb_timestamp_t, std);
  timestamp_object_handlers.get_constructor = duckdb_timestamp_constructor;
  timestamp_object_handlers.free_obj = duckdb_timestamp_free_obj;

  duckdb_date_class_entry = register_class_DuckDB_Value_Date(zend_ce_stringable);
  duckdb_date_class_entry->create_object = duckdb_date_new;
  date_object_handlers.offset = XtOffsetOf(duckdb_date_t, std);
  date_object_handlers.get_constructor = duckdb_date_constructor;
  date_object_handlers.free_obj = duckdb_date_free_obj;

  duckdb_time_class_entry = register_class_DuckDB_Value_Time(zend_ce_stringable);
  duckdb_time_class_entry->create_object = duckdb_time_new;
  time_object_handlers.offset = XtOffsetOf(duckdb_time_t, std);
  time_object_handlers.get_constructor = duckdb_time_constructor;
  time_object_handlers.free_obj = duckdb_time_free_obj;

  return SUCCESS;
}

PHP_MINFO_FUNCTION(duckdb)
{
  php_info_print_table_start();
  php_info_print_table_row(2, "Duckdb Support", "enabled");
  php_info_print_table_row(2, "Extension Version", PHP_DUCKDB_VERSION);
  php_info_print_table_row(2, "libduckdb Version", duckdb_library_version());
  php_info_print_table_end();
}

zend_module_entry duckdb_module_entry = {
  STANDARD_MODULE_HEADER,
  "duckdb",           /* Extension name */
  NULL,               /* zend_function_entry */
  PHP_MINIT(duckdb),  /* PHP_MINIT - Module initialization */
  NULL,               /* PHP_MSHUTDOWN - Module shutdown */
  PHP_RINIT(duckdb),  /* PHP_RINIT - Request initialization */
  NULL,               /* PHP_RSHUTDOWN - Request shutdown */
  PHP_MINFO(duckdb),  /* PHP_MINFO - Module info */
  PHP_DUCKDB_VERSION, /* Version */
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_DUCKDB
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(duckdb)
#endif
