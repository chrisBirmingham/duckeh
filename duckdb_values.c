#include "php.h"
#include "duckdb_structs.h"
#include "duckdb_values.h"

static inline void duckdb_string_t_to_zval(const duckdb_string_t *string, zval *data)
{
  uint32_t len = string->value.inlined.length;
  const char* ptr = (len > 12) ? string->value.pointer.ptr : string->value.inlined.inlined;
  ZVAL_STRINGL(data, ptr, len);
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
  const unsigned char *ptr = (const unsigned char *)((len > 12)
    ? string->value.pointer.ptr
    : string->value.inlined.inlined);

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
  duckdb_hugeint result = {.lower = (uint64_t)input, .upper = input < 0 ? -1 : 0};
  return result;
}

static inline duckdb_hugeint duckdb_hugeint_from_uint64(uint64_t input)
{
  duckdb_hugeint result = {.lower = input, .upper = 0};
  return result;
}

static inline void duckdb_timestamp_to_zval(zval *data, duckdb_timestamp timestamp)
{
  object_init_ex(data, duckdb_timestamp_class_entry);
  duckdb_timestamp_t *timestamp_t = Z_DUCKDB_TIMESTAMP_P(data);
  timestamp_t->timestamp = timestamp;
}

static void duckdb_timestamp_to_timestamp(duckdb_vector_t * vector_t, idx_t row_index, zval *data)
{
  duckdb_value value = NULL;
  duckdb_type type = vector_t->type;

  if (type == DUCKDB_TYPE_TIMESTAMP_S) {
    duckdb_timestamp_s timestamp_s = ((duckdb_timestamp_s *)vector_t->data)[row_index];
    value = duckdb_create_timestamp_s(timestamp_s);
  } else if (type == DUCKDB_TYPE_TIMESTAMP_MS) {
    duckdb_timestamp_ms timestamp_ms = ((duckdb_timestamp_ms *)vector_t->data)[row_index];
    duckdb_value value = duckdb_create_timestamp_ms(timestamp_ms);
  } else if (type == DUCKDB_TYPE_TIMESTAMP_NS) {
    duckdb_timestamp_ns timestamp_ns = ((duckdb_timestamp_ns *)vector_t->data)[row_index];
    duckdb_value value = duckdb_create_timestamp_ns(timestamp_ns);
  }

  duckdb_timestamp timestamp = duckdb_get_timestamp(value);
  duckdb_destroy_value(&value);
  duckdb_timestamp_to_zval(data, timestamp);
}

static void duckdb_struct_vector_to_array(duckdb_vector_t *vector_t, idx_t rowIndex, zval *array)
{
  array_init(array);
  uint64_t child_size = duckdb_struct_type_child_count(vector_t->logical_type);

  for (uint64_t i = 0; i < child_size; i++) {
    char *name = duckdb_struct_type_child_name(vector_t->logical_type, i);
    duckdb_vector child_vector = duckdb_struct_vector_get_child(vector_t->vector, i);
    duckdb_vector_t child_vector_t;
    zval data;

    duckdb_init_temp_vector(&child_vector_t, child_vector);
    duckval_to_zval(&child_vector_t, rowIndex, &data);
    add_assoc_zval(array, name, &data);
    duckdb_free(name);
    duckdb_destroy_temp_vector(&child_vector_t);
  }
}

static void duckdb_list_to_array(duckdb_vector_t *vector_t, idx_t rowIndex, zval *array)
{
  duckdb_list_entry entry = ((duckdb_list_entry *)vector_t->data)[rowIndex];
  duckdb_vector child_vector = duckdb_list_vector_get_child(vector_t->vector);
  duckdb_vector_t child_vector_t;

  duckdb_init_temp_vector(&child_vector_t, child_vector);
  array_init_size(array, (uint32_t)entry.length);

  for (uint64_t i = 0; i < entry.length; i++) {
    zval data;
    duckval_to_zval(&child_vector_t, entry.offset + i, &data);
    add_next_index_zval(array, &data);
  }

  duckdb_destroy_temp_vector(&child_vector_t);
}

static void duckdb_array_to_array(duckdb_vector_t *vector_t, idx_t rowIndex, zval *array)
{
  idx_t array_size = duckdb_array_type_array_size(vector_t->logical_type);
  duckdb_vector child_vector = duckdb_array_vector_get_child(vector_t->vector);
  duckdb_vector_t child_vector_t;
  uint64_t offset = (uint64_t)rowIndex * (uint64_t)array_size;

  duckdb_init_temp_vector(&child_vector_t, child_vector);
  array_init_size(array, (uint32_t)array_size);

  for (uint64_t i = 0; i < array_size; i++) {
    zval data;
    duckval_to_zval(&child_vector_t, offset + i, &data);
    add_next_index_zval(array, &data);
  }

  duckdb_destroy_temp_vector(&child_vector_t);
}

static void duckdb_map_to_array(duckdb_vector_t *vector_t, idx_t rowIndex, zval *array)
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
    duckval_to_zval(&key_vector_t, entry.offset + i, &key);
    duckval_to_zval(&value_vector_t, entry.offset + i, &value);

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

static void duckdb_union_to_zval(duckdb_vector_t *vector_t, idx_t rowIndex, zval *data)
{
  idx_t member_count = duckdb_union_type_member_count(vector_t->logical_type);
  for (idx_t i = 0; i < member_count; i++) {
    duckdb_vector child_vector = duckdb_struct_vector_get_child(vector_t->vector, i);
    duckdb_vector_t child_vector_t;
    zval value;

    duckdb_init_temp_vector(&child_vector_t, child_vector);
    duckval_to_zval(&child_vector_t, rowIndex, &value);
    duckdb_destroy_temp_vector(&child_vector_t);

    if (Z_TYPE(value) != IS_NULL) {
      ZVAL_COPY_VALUE(data, &value);
      return;
    }

    zval_ptr_dtor(&value);
  }

  ZVAL_NULL(data);
}

static void duckdb_value_to_zval(duckdb_vector_t *vector_t, idx_t rowIndex, zval *data)
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
        zend_string *str = zend_strpprintf(0, ZEND_ULONG_FMT, value);
        ZVAL_STR(data, str);
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
    case DUCKDB_TYPE_TIMESTAMP_TZ:
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
    case DUCKDB_TYPE_TIMESTAMP_MS:
    case DUCKDB_TYPE_TIMESTAMP_NS:
      duckdb_timestamp_to_timestamp(vector_t, rowIndex, data);
      break;
    case DUCKDB_TYPE_TIME_TZ:
    {
      duckdb_time_tz time_tz = ((duckdb_time_tz *)vector_t->data)[rowIndex];
      duckdb_time_tz_struct time_tz_struct = duckdb_from_time_tz(time_tz);
      object_init_ex(data, duckdb_time_class_entry);
      duckdb_time_t *time_t = Z_DUCKDB_TIME_P(data);
      time_t->time = time_tz_struct.time;
      break;
    }
    case DUCKDB_TYPE_TIME_NS:
    {
      duckdb_time_ns time_ns = ((duckdb_time_ns *)vector_t->data)[rowIndex];
      duckdb_value value = duckdb_create_time_ns(time_ns);
      duckdb_value_to_zval_string(value, data);
      break;
    }
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
      uint8_t *ptr = (uint8_t *)((len > 12)
        ? string->value.pointer.ptr
        : string->value.inlined.inlined);

      duckdb_bit bit_value = {.data = ptr, .size = len};
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
    default:
      ZVAL_NULL(data);
    }
}

void duckval_to_zval(duckdb_vector_t *vector_t, idx_t row_index, zval *data)
{
  if (vector_t->validity == NULL || duckdb_validity_row_is_valid(vector_t->validity, row_index)) {
    duckdb_value_to_zval(vector_t, row_index, data);
  } else {
    ZVAL_NULL(data);
  }
}

void duckdb_init_temp_vector(duckdb_vector_t *vector_t, duckdb_vector vector)
{
  vector_t->logical_type = duckdb_vector_get_column_type(vector);
  vector_t->type = duckdb_get_type_id(vector_t->logical_type);
  vector_t->validity = duckdb_vector_get_validity(vector);
  vector_t->data = duckdb_vector_get_data(vector);
  vector_t->vector = vector;
}

duckdb_value zval_to_duckval(zval *value)
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
