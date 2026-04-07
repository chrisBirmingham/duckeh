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
#include <duckdb.h>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE()  \
  ZEND_PARSE_PARAMETERS_START(0, 0) \
  ZEND_PARSE_PARAMETERS_END()
#endif

static zend_object_handlers duckdb_object_handlers;
static zend_object_handlers database_object_handlers;
static zend_object_handlers connection_object_handlers;
static zend_object_handlers prepared_statement_object_handlers;
static zend_object_handlers append_statement_object_handlers;
static zend_object_handlers result_object_handlers;
static zend_object_handlers data_chunk_object_handlers;
static zend_object_handlers vector_object_handlers;
static zend_object_handlers timestamp_object_handlers;
static zend_object_handlers date_object_handlers;
static zend_object_handlers time_object_handlers;

static zend_class_entry *duckdb_class_entry = NULL;
static zend_class_entry *duckdb_database_class_entry = NULL;
static zend_class_entry *duckdb_connection_class_entry = NULL;
static zend_class_entry *duckdb_prepared_statement_class_entry = NULL;
static zend_class_entry *duckdb_appender_class_entry = NULL;
static zend_class_entry *duckdb_result_class_entry = NULL;
static zend_class_entry *duckdb_data_chunk_class_entry = NULL;
static zend_class_entry *duckdb_vector_class_entry = NULL;
static zend_class_entry *duckdb_timestamp_class_entry = NULL;
static zend_class_entry *duckdb_date_class_entry = NULL;
static zend_class_entry *duckdb_time_class_entry = NULL;

static zend_class_entry *duckdb_exception_class_entry = NULL;
static zend_class_entry *duckdb_connection_exception_class_entry = NULL;
static zend_class_entry *duckdb_query_exception_class_entry = NULL;
static zend_class_entry *duckdb_append_exception_class_entry = NULL;

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

static void get_data(duckdb_vector_t *vector_t, zend_long row_index, zval *data);
static void duckdb_struct_vector_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array);
static void duckdb_list_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array);
static void duckdb_array_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array);
static void duckdb_map_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array);
static void duckdb_union_to_zval(duckdb_vector_t *vector_t, int rowIndex, zval *data);

static inline void duckdb_string_t_to_zval(const duckdb_string_t *string, zval *data)
{
  uint32_t len = string->value.inlined.length;

  if (len > 12) {
    ZVAL_STRINGL(data, string->value.pointer.ptr, len);
    return;
  }

  ZVAL_STRINGL(data, string->value.inlined.inlined, len);
}

static zend_string *duckdb_bignum_to_decimal_string(const unsigned char *bytes, size_t len, bool is_negative)
{
  size_t digits_cap = len * 3 + 1;
  uint8_t *digits = emalloc(digits_cap);
  size_t digits_len = 1;
  digits[0] = 0;

  for (size_t i = 0; i < len; i++) {
    uint32_t carry = is_negative ? (uint8_t)(~bytes[i]) : bytes[i];
    for (size_t j = 0; j < digits_len; j++) {
      uint32_t temp = (uint32_t)digits[j] * 256u + carry;
      digits[j] = (uint8_t)(temp % 10u);
      carry = temp / 10u;
    }

    while (carry > 0) {
      if (digits_len == digits_cap) {
        digits_cap *= 2;
        digits = erealloc(digits, digits_cap);
      }
      digits[digits_len++] = (uint8_t)(carry % 10u);
      carry /= 10u;
    }
  }

  size_t out_len = digits_len + (is_negative ? 1 : 0);
  zend_string *out = zend_string_alloc(out_len, 0);
  char *dst = ZSTR_VAL(out);
  size_t pos = 0;

  if (is_negative) {
    dst[pos++] = '-';
  }

  for (size_t i = 0; i < digits_len; i++) {
    dst[pos++] = (char)('0' + digits[digits_len - 1 - i]);
  }

  dst[pos] = '\0';
  efree(digits);
  return out;
}

static zend_string *duckdb_string_t_to_bignum_string(const duckdb_string_t *string)
{
  uint32_t len = string->value.inlined.length;
  const unsigned char *ptr = NULL;

  if (len > 12) {
    ptr = (const unsigned char *)string->value.pointer.ptr;
  } else {
    ptr = (const unsigned char *)string->value.inlined.inlined;
  }

  bool is_negative = (len > 1 && ptr[1] != 0);
  if (len <= 3) {
    return duckdb_bignum_to_decimal_string(NULL, 0, is_negative);
  }

  return duckdb_bignum_to_decimal_string(ptr + 3, (size_t)len - 3, is_negative);
}

static inline void duckdb_value_to_zval_string(duckdb_value value, zval *data)
{
  char *string = duckdb_get_varchar(value);

  if (string != NULL) {
    ZVAL_STRING(data, string);
    duckdb_free(string);
  } else {
    ZVAL_NULL(data);
  }

  duckdb_destroy_value(&value);
}

static inline duckdb_hugeint duckdb_hugeint_from_int64(int64_t input)
{
  duckdb_hugeint result;
  result.lower = (uint64_t)input;
  result.upper = input < 0 ? -1 : 0;
  return result;
}

static inline duckdb_hugeint duckdb_hugeint_from_uint64(uint64_t input)
{
  duckdb_hugeint result;
  result.lower = input;
  result.upper = 0;
  return result;
}

static inline void duckdb_timestamp_to_zval(zval *data, duckdb_timestamp timestamp)
{
  object_init_ex(data, duckdb_timestamp_class_entry);
  duckdb_timestamp_t *timestamp_t = Z_DUCKDB_TIMESTAMP_P(data);
  timestamp_t->timestamp = timestamp;
}

static void duckdb_value_to_zval(duckdb_vector_t *vector_t, int rowIndex, zval *data)
{
  switch (vector_t->type) {
    case DUCKDB_TYPE_BOOLEAN:
      ZVAL_BOOL(data, ((bool *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_VARCHAR:
      duckdb_string_t_to_zval(&((duckdb_string_t *)vector_t->data)[rowIndex], data);
      break;
    case DUCKDB_TYPE_TINYINT:
      ZVAL_LONG(data, ((int8_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_SMALLINT:
      ZVAL_LONG(data, ((int16_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_INTEGER:
      ZVAL_LONG(data, ((int32_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_BIGINT:
      ZVAL_LONG(data, ((int64_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_UTINYINT:
      ZVAL_LONG(data, ((uint8_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_USMALLINT:
      ZVAL_LONG(data, ((uint16_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_UINTEGER:
      ZVAL_LONG(data, ((uint32_t *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_UBIGINT:
    {
      uint64_t value = ((uint64_t *)vector_t->data)[rowIndex];
      if (value <= (uint64_t)ZEND_LONG_MAX) {
        ZVAL_LONG(data, (zend_long)value);
      } else {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%" PRIu64, value);
        ZVAL_STRING(data, buffer);
      }
      break;
    }
    case DUCKDB_TYPE_FLOAT:
      ZVAL_DOUBLE(data, ((float *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_DOUBLE:
      ZVAL_DOUBLE(data, ((double *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_TIMESTAMP:
      duckdb_timestamp_to_zval(data, ((duckdb_timestamp *)vector_t->data)[rowIndex]);
      break;
    case DUCKDB_TYPE_DATE:
    {
      object_init_ex(data, duckdb_date_class_entry);
      duckdb_date_t *date_t = Z_DUCKDB_DATE_P(data);
      date_t->date = ((duckdb_date *)vector_t->data)[rowIndex];
      break;
    }
    case DUCKDB_TYPE_TIME:
    {
      object_init_ex(data, duckdb_time_class_entry);
      duckdb_time_t *time_t = Z_DUCKDB_TIME_P(data);
      time_t->time = duckdb_from_time(((duckdb_time *)vector_t->data)[rowIndex]);
      break;
    }
    case DUCKDB_TYPE_TIMESTAMP_S:
    {
      duckdb_timestamp_s timestamp_s = ((duckdb_timestamp_s *)vector_t->data)[rowIndex];
      duckdb_value value = duckdb_create_timestamp_s(timestamp_s);
      duckdb_timestamp timestamp = duckdb_get_timestamp(value);
      duckdb_destroy_value(&value);
      duckdb_timestamp_to_zval(data, timestamp);
      break;
    }
    case DUCKDB_TYPE_TIMESTAMP_MS:
    {
      duckdb_timestamp_ms timestamp_ms = ((duckdb_timestamp_ms *)vector_t->data)[rowIndex];
      duckdb_value value = duckdb_create_timestamp_ms(timestamp_ms);
      duckdb_timestamp timestamp = duckdb_get_timestamp(value);
      duckdb_destroy_value(&value);
      duckdb_timestamp_to_zval(data, timestamp);
      break;
    }
    case DUCKDB_TYPE_TIMESTAMP_NS:
    {
      duckdb_timestamp_ns timestamp_ns = ((duckdb_timestamp_ns *)vector_t->data)[rowIndex];
      duckdb_value value = duckdb_create_timestamp_ns(timestamp_ns);
      duckdb_timestamp timestamp = duckdb_get_timestamp(value);
      duckdb_destroy_value(&value);
      duckdb_timestamp_to_zval(data, timestamp);
      break;
    }
    case DUCKDB_TYPE_TIMESTAMP_TZ:
    {
      duckdb_timestamp timestamp_tz = ((duckdb_timestamp *)vector_t->data)[rowIndex];
      duckdb_value value = duckdb_create_timestamp_tz(timestamp_tz);
      duckdb_timestamp timestamp = duckdb_get_timestamp(value);
      duckdb_destroy_value(&value);
      duckdb_timestamp_to_zval(data, timestamp);
      break;
    }
    case DUCKDB_TYPE_TIME_TZ:
    {
      duckdb_time_tz time_tz = ((duckdb_time_tz *)vector_t->data)[rowIndex];
      duckdb_time_tz_struct time_tz_struct = duckdb_from_time_tz(time_tz);
      object_init_ex(data, duckdb_time_class_entry);
      duckdb_time_t *time_t = Z_DUCKDB_TIME_P(data);
      time_t->time = time_tz_struct.time;
      break;
    }
#ifdef DUCKDB_TYPE_TIME_NS
    case DUCKDB_TYPE_TIME_NS:
    {
      duckdb_time_ns time_ns = ((duckdb_time_ns *)vector_t->data)[rowIndex];
      duckdb_value value = duckdb_create_time_ns(time_ns);
      duckdb_value_to_zval_string(value, data);
      break;
    }
#endif
    case DUCKDB_TYPE_STRUCT:
      duckdb_struct_vector_to_array(vector_t, rowIndex, data);
      break;
    case DUCKDB_TYPE_LIST:
      duckdb_list_to_array(vector_t, rowIndex, data);
      break;
    case DUCKDB_TYPE_ARRAY:
      duckdb_array_to_array(vector_t, rowIndex, data);
      break;
    case DUCKDB_TYPE_MAP:
      duckdb_map_to_array(vector_t, rowIndex, data);
      break;
    case DUCKDB_TYPE_ENUM:
    {
      duckdb_type internal_type = duckdb_enum_internal_type(vector_t->logical_type);
      uint64_t enum_index = 0;
      switch (internal_type) {
        case DUCKDB_TYPE_UTINYINT:
          enum_index = ((uint8_t *)vector_t->data)[rowIndex];
          break;
        case DUCKDB_TYPE_USMALLINT:
          enum_index = ((uint16_t *)vector_t->data)[rowIndex];
          break;
        case DUCKDB_TYPE_UINTEGER:
          enum_index = ((uint32_t *)vector_t->data)[rowIndex];
          break;
        case DUCKDB_TYPE_UBIGINT:
          enum_index = ((uint64_t *)vector_t->data)[rowIndex];
          break;
        default:
          ZVAL_NULL(data);
          return;
        }

        uint32_t dictionary_size = duckdb_enum_dictionary_size(vector_t->logical_type);
        if (enum_index >= dictionary_size) {
          ZVAL_NULL(data);
          break;
        }

        char *enum_value = duckdb_enum_dictionary_value(vector_t->logical_type, (idx_t)enum_index);
        if (enum_value == NULL) {
          ZVAL_NULL(data);
          break;
        }

        ZVAL_STRING(data, enum_value);
        duckdb_free(enum_value);
        break;
    }
    case DUCKDB_TYPE_DECIMAL:
    {
      duckdb_decimal decimal;
      decimal.width = duckdb_decimal_width(vector_t->logical_type);
      decimal.scale = duckdb_decimal_scale(vector_t->logical_type);

      switch (duckdb_decimal_internal_type(vector_t->logical_type)) {
        case DUCKDB_TYPE_TINYINT:
          decimal.value = duckdb_hugeint_from_int64(((int8_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_SMALLINT:
          decimal.value = duckdb_hugeint_from_int64(((int16_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_INTEGER:
          decimal.value = duckdb_hugeint_from_int64(((int32_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_BIGINT:
          decimal.value = duckdb_hugeint_from_int64(((int64_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_UTINYINT:
          decimal.value = duckdb_hugeint_from_uint64(((uint8_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_USMALLINT:
          decimal.value = duckdb_hugeint_from_uint64(((uint16_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_UINTEGER:
          decimal.value = duckdb_hugeint_from_uint64(((uint32_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_UBIGINT:
          decimal.value = duckdb_hugeint_from_uint64(((uint64_t *)vector_t->data)[rowIndex]);
          break;
        case DUCKDB_TYPE_HUGEINT:
          decimal.value = ((duckdb_hugeint *)vector_t->data)[rowIndex];
          break;
        default:
          decimal.value = duckdb_hugeint_from_int64(0);
          break;
        }

        ZVAL_DOUBLE(data, duckdb_decimal_to_double(decimal));
        break;
    }
    case DUCKDB_TYPE_HUGEINT:
    {
      duckdb_hugeint value = ((duckdb_hugeint *)vector_t->data)[rowIndex];
      duckdb_value value_value = duckdb_create_hugeint(value);
      duckdb_value_to_zval_string(value_value, data);
      break;
    }
    case DUCKDB_TYPE_UHUGEINT:
    {
      duckdb_uhugeint value = ((duckdb_uhugeint *)vector_t->data)[rowIndex];
      duckdb_value value_value = duckdb_create_uhugeint(value);
      duckdb_value_to_zval_string(value_value, data);
      break;
    }
    case DUCKDB_TYPE_BLOB:
      duckdb_string_t_to_zval(&((duckdb_string_t *)vector_t->data)[rowIndex], data);
      break;
    case DUCKDB_TYPE_BIT:
    {
      duckdb_string_t *string = &((duckdb_string_t *)vector_t->data)[rowIndex];
      uint32_t len = string->value.inlined.length;
      uint8_t *ptr = NULL;
      if (len > 12) {
        ptr = (uint8_t *)string->value.pointer.ptr;
      } else {
        ptr = (uint8_t *)string->value.inlined.inlined;
      }

      duckdb_bit bit_value;
      bit_value.data = ptr;
      bit_value.size = len;
      duckdb_value value = duckdb_create_bit(bit_value);
      duckdb_value_to_zval_string(value, data);
      break;
    }
    case DUCKDB_TYPE_BIGNUM:
    {
      duckdb_string_t *string = &((duckdb_string_t *)vector_t->data)[rowIndex];
      zend_string *value_string = duckdb_string_t_to_bignum_string(string);
      ZVAL_STR(data, value_string);
      break;
    }
    case DUCKDB_TYPE_UUID:
    {
      duckdb_hugeint uuid_hugeint = ((duckdb_hugeint *)vector_t->data)[rowIndex];
      duckdb_uhugeint uuid_uhugeint;
      uuid_uhugeint.lower = uuid_hugeint.lower;

      if (uuid_hugeint.upper >= 0) {
        uuid_uhugeint.upper = (uint64_t)(uuid_hugeint.upper) + (uint64_t)(INT64_MAX) + 1;
      } else {
        uuid_uhugeint.upper = (uint64_t)(uuid_hugeint.upper + INT64_MAX + 1);
      }

      duckdb_value value = duckdb_create_uuid(uuid_uhugeint);
      duckdb_value_to_zval_string(value, data);
      break;
    }
    case DUCKDB_TYPE_INTERVAL:
    {
      duckdb_interval value = ((duckdb_interval *)vector_t->data)[rowIndex];
      duckdb_value value_value = duckdb_create_interval(value);
      duckdb_value_to_zval_string(value_value, data);
      break;
    }
    case DUCKDB_TYPE_UNION:
      duckdb_union_to_zval(vector_t, rowIndex, data);
      break;
    case DUCKDB_TYPE_SQLNULL:
    case DUCKDB_TYPE_ANY:
      ZVAL_NULL(data);
      break;
    default:
      ZVAL_NULL(data);
    }
}

static void get_data(duckdb_vector_t *vector_t, zend_long row_index, zval *data)
{
  if (vector_t->type == DUCKDB_TYPE_INVALID) {
    vector_t->logical_type = duckdb_vector_get_column_type(vector_t->vector);
    vector_t->type = duckdb_get_type_id(vector_t->logical_type);
  }

  if (vector_t->validity == NULL) {
    vector_t->validity = duckdb_vector_get_validity(vector_t->vector);
  }

  if (vector_t->data == NULL) {
    vector_t->data = duckdb_vector_get_data(vector_t->vector);
  }

  if (vector_t->validity == NULL) {
    duckdb_value_to_zval(vector_t, row_index, data);
    return;
  }

  if (duckdb_validity_row_is_valid(vector_t->validity, row_index)) {
    duckdb_value_to_zval(vector_t, row_index, data);
    return;
  }

  ZVAL_NULL(data);
}

static inline void duckdb_init_temp_vector(duckdb_vector_t *vector_t, duckdb_vector vector)
{
  memset(vector_t, 0, sizeof(*vector_t));
  vector_t->vector = vector;
}

static inline void duckdb_destroy_temp_vector(duckdb_vector_t *vector_t)
{
  if (vector_t->logical_type != NULL) {
    duckdb_destroy_logical_type(&vector_t->logical_type);
  }
}

static void duckdb_struct_vector_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array)
{
  if (vector_t->logical_type == NULL) {
    vector_t->logical_type = duckdb_vector_get_column_type(vector_t->vector);
  }

  array_init(array);
  uint64_t child_size = duckdb_struct_type_child_count(vector_t->logical_type);

  for (uint64_t i = 0; i < child_size; i++) {
    char *name = duckdb_struct_type_child_name(vector_t->logical_type, i);
    duckdb_vector child_vector = duckdb_struct_vector_get_child(vector_t->vector, i);
    duckdb_vector_t child_vector_t;
    zval data;

    duckdb_init_temp_vector(&child_vector_t, child_vector);
    get_data(&child_vector_t, rowIndex, &data);
    add_assoc_zval(array, name, &data);
    duckdb_free(name);
    duckdb_destroy_temp_vector(&child_vector_t);
  }
}

static void duckdb_list_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array)
{
  duckdb_list_entry entry = ((duckdb_list_entry *)vector_t->data)[rowIndex];
  duckdb_vector child_vector = duckdb_list_vector_get_child(vector_t->vector);
  duckdb_vector_t child_vector_t;

  duckdb_init_temp_vector(&child_vector_t, child_vector);
  array_init_size(array, (uint32_t)entry.length);

  for (uint64_t i = 0; i < entry.length; i++) {
    zval data;
    get_data(&child_vector_t, entry.offset + i, &data);
    add_next_index_zval(array, &data);
  }

  duckdb_destroy_temp_vector(&child_vector_t);
}

static void duckdb_array_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array)
{
  if (vector_t->logical_type == NULL) {
    vector_t->logical_type = duckdb_vector_get_column_type(vector_t->vector);
  }

  idx_t array_size = duckdb_array_type_array_size(vector_t->logical_type);
  duckdb_vector child_vector = duckdb_array_vector_get_child(vector_t->vector);
  duckdb_vector_t child_vector_t;
  uint64_t offset = (uint64_t)rowIndex * (uint64_t)array_size;

  duckdb_init_temp_vector(&child_vector_t, child_vector);
  array_init_size(array, (uint32_t)array_size);

  for (uint64_t i = 0; i < array_size; i++) {
    zval data;
    get_data(&child_vector_t, offset + i, &data);
    add_next_index_zval(array, &data);
  }

  duckdb_destroy_temp_vector(&child_vector_t);
}

static void duckdb_map_to_array(duckdb_vector_t *vector_t, int rowIndex, zval *array)
{
  duckdb_list_entry entry = ((duckdb_list_entry *)vector_t->data)[rowIndex];
  duckdb_vector child_vector = duckdb_list_vector_get_child(vector_t->vector);
  duckdb_vector key_vector = duckdb_struct_vector_get_child(child_vector, 0);
  duckdb_vector value_vector = duckdb_struct_vector_get_child(child_vector, 1);
  duckdb_vector_t key_vector_t;
  duckdb_vector_t value_vector_t;

  duckdb_init_temp_vector(&key_vector_t, key_vector);
  duckdb_init_temp_vector(&value_vector_t, value_vector);
  array_init_size(array, (uint32_t)entry.length);

  for (uint64_t i = 0; i < entry.length; i++) {
    zval key;
    zval value;
    get_data(&key_vector_t, entry.offset + i, &key);
    get_data(&value_vector_t, entry.offset + i, &value);

    if (Z_TYPE(key) == IS_LONG) {
      add_index_zval(array, Z_LVAL(key), &value);
    } else if (Z_TYPE(key) == IS_STRING) {
      add_assoc_zval_ex(array, Z_STRVAL(key), Z_STRLEN(key), &value);
    } else {
      zend_string *key_str = zval_get_string(&key);
      add_assoc_zval_ex(array, ZSTR_VAL(key_str), ZSTR_LEN(key_str), &value);
      zend_string_release(key_str);
    }

    zval_ptr_dtor(&key);
  }

  duckdb_destroy_temp_vector(&key_vector_t);
  duckdb_destroy_temp_vector(&value_vector_t);
}

static void duckdb_union_to_zval(duckdb_vector_t *vector_t, int rowIndex, zval *data)
{
  if (vector_t->logical_type == NULL) {
    vector_t->logical_type = duckdb_vector_get_column_type(vector_t->vector);
  }

  idx_t member_count = duckdb_union_type_member_count(vector_t->logical_type);
  for (idx_t i = 0; i < member_count; i++) {
    duckdb_vector child_vector = duckdb_struct_vector_get_child(vector_t->vector, i);
    duckdb_vector_t child_vector_t;
    zval value;

    duckdb_init_temp_vector(&child_vector_t, child_vector);
    get_data(&child_vector_t, rowIndex, &value);
    duckdb_destroy_temp_vector(&child_vector_t);

    if (Z_TYPE(value) != IS_NULL) {
      ZVAL_COPY_VALUE(data, &value);
      return;
    }

    zval_ptr_dtor(&value);
  }

  ZVAL_NULL(data);
}

PHP_METHOD(DuckDB_DuckDB, query)
{
  zval *object = ZEND_THIS;
  duckdb_t *duckdb_t;
  duckdb_result_t *result_t;
  duckdb_result res;
  char *query = NULL;
  size_t query_len = 0;

  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(query, query_len)
  ZEND_PARSE_PARAMETERS_END();

  duckdb_t = Z_DUCKDB_P(object);

  if (duckdb_query(duckdb_t->connection, query, &res) == DuckDBError) {
    zend_throw_exception(duckdb_query_exception_class_entry, duckdb_result_error(&res), 0);
    duckdb_destroy_result(&res);
    RETURN_THROWS();
  }

  object_init_ex(return_value, duckdb_result_class_entry);
  result_t = Z_DUCKDB_RESULT_P(return_value);
  result_t->result = res;
}

PHP_METHOD(DuckDB_DuckDB, prepare)
{
  zval *object = ZEND_THIS;
  duckdb_t *duckdb_t;
  char *query = NULL;
  size_t query_len = 0;
  duckdb_prepared_statement stmt;

  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(query, query_len)
  ZEND_PARSE_PARAMETERS_END();

  duckdb_t = Z_DUCKDB_P(object);

  if (duckdb_prepare(duckdb_t->connection, query, &stmt) == DuckDBError) {
    zend_throw_exception(duckdb_query_exception_class_entry, duckdb_prepare_error(stmt), 0);
    duckdb_destroy_prepare(&stmt);
    RETURN_THROWS();
  }

  object_init_ex(return_value, duckdb_prepared_statement_class_entry);
  duckdb_prepared_statement_t *prepared_statement_t = Z_PREPARED_STATEMENT_P(return_value);
  prepared_statement_t->stmt = stmt;
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

  ZEND_PARSE_PARAMETERS_START(1, 2)
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

static duckdb_value zval_to_duckval(zval *value)
{
  switch (Z_TYPE_P(value)) {
    case IS_LONG:
      return duckdb_create_int64(Z_LVAL_P(value));
    case IS_DOUBLE:
      return duckdb_create_double(Z_DVAL_P(value));
    case IS_TRUE:
      return duckdb_create_bool(true);
    case IS_FALSE:
      return duckdb_create_bool(false);
    case IS_STRING:
      return duckdb_create_varchar(Z_STRVAL_P(value));
    case IS_NULL:
      return duckdb_create_null_value();
    default:
      return NULL;
  }
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
  zval *row = NULL;

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
  zval *row = NULL;

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

PHP_METHOD(DuckDB_Result, rowCount)
{
  zval *object = ZEND_THIS;
  duckdb_result_t *result_t;

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);
  RETURN_LONG(duckdb_rows_changed(&result_t->result));
}

PHP_METHOD(DuckDB_Result, columnCount)
{
  zval *object = ZEND_THIS;
  duckdb_result_t *result_t;

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);
  RETURN_LONG(duckdb_column_count(&result_t->result));
}

PHP_METHOD(DuckDB_Result, fetchChunk)
{
  zval *object = ZEND_THIS;
  duckdb_result_t *result_t;
  duckdb_data_chunk_t *data_chunk_t;
  duckdb_data_chunk chunk;

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);

  chunk = duckdb_fetch_chunk(result_t->result);
  if (!chunk) {
    RETURN_FALSE;
  }

  object_init_ex(return_value, duckdb_data_chunk_class_entry);
  data_chunk_t = Z_DUCKDB_DATA_CHUNK_P(return_value);
  data_chunk_t->chunk = chunk;
}

static void fetch_row(zval *arr, duckdb_result *res, duckdb_data_chunk chunk, idx_t column_count, idx_t row)
{
  for (idx_t c = 0; c < column_count; c++) {
    const char *name = duckdb_column_name(res, c);
    duckdb_vector_t vector;
    duckdb_init_temp_vector(&vector, duckdb_data_chunk_get_vector(chunk, c));

    zval value;
    get_data(&vector, (zend_long)row, &value);
    add_assoc_zval(arr, name, &value);
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

  ZEND_PARSE_PARAMETERS_NONE();

  result_t = Z_DUCKDB_RESULT_P(object);
  idx_t column_count = duckdb_column_count(&result_t->result);
  array_init(return_value);

  for (;;) {
    duckdb_data_chunk chunk = duckdb_fetch_chunk(result_t->result);

    if (!chunk) {
      break;
    }

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

static zend_object *duckdb_data_chunk_new(zend_class_entry *ce)
{
  duckdb_data_chunk_t *data_chunk = zend_object_alloc(sizeof(duckdb_data_chunk_t), ce);
  zend_object_std_init(&data_chunk->std, ce);
  object_properties_init(&data_chunk->std, ce);
  data_chunk->std.handlers = &data_chunk_object_handlers;
  return &data_chunk->std;
}

static void duckdb_data_chunk_free_obj(zend_object *obj)
{
  duckdb_data_chunk_t *data_chunk = duckdb_data_chunk_t_from_obj(obj);
  duckdb_destroy_data_chunk(&data_chunk->chunk);
  zend_object_std_dtor(&data_chunk->std);
}

static zend_function *duckdb_data_chunk_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Data Chuck. use Result::fetchChunk");
  return NULL;
}

PHP_METHOD(DuckDB_DataChunk, getSize)
{
  zval *object = ZEND_THIS;
  duckdb_data_chunk_t *data_chunk_t;

  ZEND_PARSE_PARAMETERS_NONE();

  data_chunk_t = Z_DUCKDB_DATA_CHUNK_P(object);
  RETURN_LONG(duckdb_data_chunk_get_size(data_chunk_t->chunk));
}

PHP_METHOD(DuckDB_DataChunk, getVector)
{
  zval *object = ZEND_THIS;
  duckdb_data_chunk_t *data_chunk_t;
  duckdb_vector_t *vector_t;
  zend_long index;

  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(index)
  ZEND_PARSE_PARAMETERS_END();

  if (index < 0) {
    zend_argument_value_error(1, "must be greater than or equal to 0");
    RETURN_THROWS();
  }

  data_chunk_t = Z_DUCKDB_DATA_CHUNK_P(object);

  object_init_ex(return_value, duckdb_vector_class_entry);
  vector_t = Z_DUCKDB_VECTOR_P(return_value);
  vector_t->vector = duckdb_data_chunk_get_vector(data_chunk_t->chunk, index);
}

static zend_object *duckdb_vector_new(zend_class_entry *ce)
{
  duckdb_vector_t *vector = zend_object_alloc(sizeof(duckdb_vector_t), ce);
  zend_object_std_init(&vector->std, ce);
  object_properties_init(&vector->std, ce);
  vector->std.handlers = &vector_object_handlers;
  vector->vector = NULL;
  vector->type = DUCKDB_TYPE_INVALID;
  vector->logical_type = NULL;
  vector->data = NULL;
  vector->validity = NULL;
  return &vector->std;
}

static void duckdb_vector_free_obj(zend_object *obj)
{
  duckdb_vector_t *vector = duckdb_vector_t_from_obj(obj);

  if (vector->logical_type != NULL) {
    duckdb_destroy_logical_type(&vector->logical_type);
  }

  zend_object_std_dtor(&vector->std);
}

static zend_function *duckdb_vector_constructor(zend_object *obj)
{
  zend_throw_error(NULL, "You cannot directly instantiate a DuckDB Vector, use DataChunk::getVector");
  return NULL;
}

PHP_METHOD(DuckDB_Vector, getData)
{
  zval *object = ZEND_THIS;
  duckdb_vector_t *vector_t;
  zend_long rowIndex;

  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(rowIndex)
  ZEND_PARSE_PARAMETERS_END();

  if (rowIndex < 0) {
    zend_argument_value_error(1, "must be greater than or equal to 0");
    RETURN_THROWS();
  }

  vector_t = Z_DUCKDB_VECTOR_P(object);
  get_data(vector_t, rowIndex, return_value);
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

  if ((&timestamp_t->timestamp)->micros > 0) {
    RETURN_LONG(PHP_DUCKDB_POSITIVE_INFINITY);
  }

  RETURN_LONG(PHP_DUCKDB_NEGATIVE_INFINITY);
}

PHP_METHOD(DuckDB_Value_Timestamp, getDate)
{
  zval *object = ZEND_THIS;
  duckdb_timestamp_t *timestamp_t;
  duckdb_timestamp_struct timestamp_struct;

  ZEND_PARSE_PARAMETERS_NONE();

  timestamp_t = Z_DUCKDB_TIMESTAMP_P(object);

  timestamp_struct = duckdb_from_timestamp(timestamp_t->timestamp);
  object_init_ex(return_value, duckdb_date_class_entry);
  duckdb_date_t *date_t = Z_DUCKDB_DATE_P(return_value);
  date_t->date = duckdb_to_date(timestamp_struct.date);
}

PHP_METHOD(DuckDB_Value_Timestamp, getTime)
{
  zval *object = ZEND_THIS;
  duckdb_timestamp_t *timestamp_t;
  duckdb_timestamp_struct timestamp_struct;

  ZEND_PARSE_PARAMETERS_NONE();

  timestamp_t = Z_DUCKDB_TIMESTAMP_P(object);

  timestamp_struct = duckdb_from_timestamp(timestamp_t->timestamp);
  object_init_ex(return_value, duckdb_time_class_entry);
  duckdb_time_t *time_t = Z_DUCKDB_TIME_P(return_value);
  time_t->time = timestamp_struct.time;
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

  if (*&(&date_t->date)->days > 0) {
    RETURN_LONG(PHP_DUCKDB_POSITIVE_INFINITY);
  }

  RETURN_LONG(PHP_DUCKDB_NEGATIVE_INFINITY);
}

static duckdb_date_struct get_date_struct_from_date(duckdb_date_t *date)
{
  if (date->date_struct_initialised != true) {
    date->date_struct = duckdb_from_date(date->date);
    date->date_struct_initialised = true;
  }
  return date->date_struct;
}

PHP_METHOD(DuckDB_Value_Date, getYear)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;
  duckdb_date_struct date_struct;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);

  date_struct = get_date_struct_from_date(date_t);

  RETURN_LONG(date_struct.year);
}

PHP_METHOD(DuckDB_Value_Date, getMonth)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;
  duckdb_date_struct date_struct;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);

  date_struct = get_date_struct_from_date(date_t);

  RETURN_LONG(date_struct.month);
}

PHP_METHOD(DuckDB_Value_Date, getDay)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;
  duckdb_date_struct date_struct;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);

  date_struct = get_date_struct_from_date(date_t);

  RETURN_LONG(date_struct.day);
}

PHP_METHOD(DuckDB_Value_Date, getDays)
{
  zval *object = ZEND_THIS;
  duckdb_date_t *date_t;

  ZEND_PARSE_PARAMETERS_NONE();

  date_t = Z_DUCKDB_DATE_P(object);

  RETURN_LONG((&date_t->date)->days);
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

PHP_METHOD(DuckDB_Value_Time, getHour)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);

  RETURN_LONG(time_t->time.hour);
}

PHP_METHOD(DuckDB_Value_Time, getMinutes)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);

  RETURN_LONG(time_t->time.min);
}

PHP_METHOD(DuckDB_Value_Time, getSeconds)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);

  RETURN_LONG(time_t->time.sec);
}

PHP_METHOD(DuckDB_Value_Time, getMicroseconds)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);

  RETURN_LONG(time_t->time.micros);
}

PHP_METHOD(DuckDB_Value_Time, getTotalMicroseconds)
{
  zval *object = ZEND_THIS;
  duckdb_time_t *time_t;
  duckdb_time time;

  ZEND_PARSE_PARAMETERS_NONE();

  time_t = Z_DUCKDB_TIME_P(object);
  time = duckdb_to_time(time_t->time);

  RETURN_LONG(time.micros);
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

  str = zend_strpprintf(0, "%02d:%02d:%02d.%06d", time.hour, time.min, time.sec, time.micros);
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
  memcpy(&data_chunk_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
  memcpy(&vector_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
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

  duckdb_data_chunk_class_entry = register_class_DuckDB_DataChunk();
  duckdb_data_chunk_class_entry->create_object = duckdb_data_chunk_new;
  data_chunk_object_handlers.offset = XtOffsetOf(duckdb_data_chunk_t, std);
  data_chunk_object_handlers.get_constructor = duckdb_data_chunk_constructor;
  data_chunk_object_handlers.free_obj = duckdb_data_chunk_free_obj;

  duckdb_vector_class_entry = register_class_DuckDB_Vector();
  duckdb_vector_class_entry->create_object = duckdb_vector_new;
  vector_object_handlers.offset = XtOffsetOf(duckdb_vector_t, std);
  vector_object_handlers.get_constructor = duckdb_vector_constructor;
  vector_object_handlers.free_obj = duckdb_vector_free_obj;

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
