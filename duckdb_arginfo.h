/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4a8c299927d7c46b92cc331c43b0f11de6a84ed4 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DuckDB_DuckDB___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DuckDB_DuckDB_query, 0, 1, DuckDB\\Result, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DuckDB_DuckDB_prepare, 0, 1, DuckDB\\PreparedStatement, 0)
	ZEND_ARG_TYPE_INFO(0, query, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DuckDB_DuckDB_append, 0, 1, DuckDB\\Appender, 0)
	ZEND_ARG_TYPE_INFO(0, table, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, schema, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, catalogue, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DuckDB_Result_columnCount, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DuckDB_Result_rowCount arginfo_class_DuckDB_Result_columnCount

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_DuckDB_Result_fetch, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DuckDB_Result_fetchAll, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DuckDB_PreparedStatement_bindParam, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_MASK(0, param, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_TYPE_INFO(0, val, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DuckDB_PreparedStatement_execute, 0, 0, DuckDB\\Result, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, params, IS_ARRAY, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DuckDB_Appender_appendRow, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, row, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DuckDB_Appender_flush, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DuckDB_Appender_clear arginfo_class_DuckDB_Appender_flush

#define arginfo_class_DuckDB_Value_Timestamp_infinity arginfo_class_DuckDB_Result_columnCount

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DuckDB_Value_Timestamp_getDate, 0, 0, DuckDB\\Value\\Date, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_DuckDB_Value_Timestamp_getTime, 0, 0, DuckDB\\Value\\Time, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DuckDB_Value_Timestamp___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DuckDB_Value_Date_infinity arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Date_getYear arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Date_getMonth arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Date_getDay arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Date_getDays arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Date___toString arginfo_class_DuckDB_Value_Timestamp___toString

#define arginfo_class_DuckDB_Value_Time_getTotalMicroseconds arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Time_getHour arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Time_getMinutes arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Time_getSeconds arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Time_getOffset arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Time_getMicroseconds arginfo_class_DuckDB_Result_columnCount

#define arginfo_class_DuckDB_Value_Time___toString arginfo_class_DuckDB_Value_Timestamp___toString

ZEND_METHOD(DuckDB_DuckDB, __construct);
ZEND_METHOD(DuckDB_DuckDB, query);
ZEND_METHOD(DuckDB_DuckDB, prepare);
ZEND_METHOD(DuckDB_DuckDB, append);
ZEND_METHOD(DuckDB_Result, columnCount);
ZEND_METHOD(DuckDB_Result, rowCount);
ZEND_METHOD(DuckDB_Result, fetch);
ZEND_METHOD(DuckDB_Result, fetchAll);
ZEND_METHOD(DuckDB_PreparedStatement, bindParam);
ZEND_METHOD(DuckDB_PreparedStatement, execute);
ZEND_METHOD(DuckDB_Appender, appendRow);
ZEND_METHOD(DuckDB_Appender, flush);
ZEND_METHOD(DuckDB_Appender, clear);
ZEND_METHOD(DuckDB_Value_Timestamp, infinity);
ZEND_METHOD(DuckDB_Value_Timestamp, getDate);
ZEND_METHOD(DuckDB_Value_Timestamp, getTime);
ZEND_METHOD(DuckDB_Value_Timestamp, __toString);
ZEND_METHOD(DuckDB_Value_Date, infinity);
ZEND_METHOD(DuckDB_Value_Date, getYear);
ZEND_METHOD(DuckDB_Value_Date, getMonth);
ZEND_METHOD(DuckDB_Value_Date, getDay);
ZEND_METHOD(DuckDB_Value_Date, getDays);
ZEND_METHOD(DuckDB_Value_Date, __toString);
ZEND_METHOD(DuckDB_Value_Time, getTotalMicroseconds);
ZEND_METHOD(DuckDB_Value_Time, getHour);
ZEND_METHOD(DuckDB_Value_Time, getMinutes);
ZEND_METHOD(DuckDB_Value_Time, getSeconds);
ZEND_METHOD(DuckDB_Value_Time, getOffset);
ZEND_METHOD(DuckDB_Value_Time, getMicroseconds);
ZEND_METHOD(DuckDB_Value_Time, __toString);

static const zend_function_entry class_DuckDB_DuckDB_methods[] = {
	ZEND_ME(DuckDB_DuckDB, __construct, arginfo_class_DuckDB_DuckDB___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_DuckDB, query, arginfo_class_DuckDB_DuckDB_query, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_DuckDB, prepare, arginfo_class_DuckDB_DuckDB_prepare, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_DuckDB, append, arginfo_class_DuckDB_DuckDB_append, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DuckDB_Result_methods[] = {
	ZEND_ME(DuckDB_Result, columnCount, arginfo_class_DuckDB_Result_columnCount, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Result, rowCount, arginfo_class_DuckDB_Result_rowCount, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Result, fetch, arginfo_class_DuckDB_Result_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Result, fetchAll, arginfo_class_DuckDB_Result_fetchAll, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DuckDB_PreparedStatement_methods[] = {
	ZEND_ME(DuckDB_PreparedStatement, bindParam, arginfo_class_DuckDB_PreparedStatement_bindParam, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_PreparedStatement, execute, arginfo_class_DuckDB_PreparedStatement_execute, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DuckDB_Appender_methods[] = {
	ZEND_ME(DuckDB_Appender, appendRow, arginfo_class_DuckDB_Appender_appendRow, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Appender, flush, arginfo_class_DuckDB_Appender_flush, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Appender, clear, arginfo_class_DuckDB_Appender_clear, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DuckDB_Value_Timestamp_methods[] = {
	ZEND_ME(DuckDB_Value_Timestamp, infinity, arginfo_class_DuckDB_Value_Timestamp_infinity, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Timestamp, getDate, arginfo_class_DuckDB_Value_Timestamp_getDate, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Timestamp, getTime, arginfo_class_DuckDB_Value_Timestamp_getTime, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Timestamp, __toString, arginfo_class_DuckDB_Value_Timestamp___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DuckDB_Value_Date_methods[] = {
	ZEND_ME(DuckDB_Value_Date, infinity, arginfo_class_DuckDB_Value_Date_infinity, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Date, getYear, arginfo_class_DuckDB_Value_Date_getYear, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Date, getMonth, arginfo_class_DuckDB_Value_Date_getMonth, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Date, getDay, arginfo_class_DuckDB_Value_Date_getDay, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Date, getDays, arginfo_class_DuckDB_Value_Date_getDays, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Date, __toString, arginfo_class_DuckDB_Value_Date___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static const zend_function_entry class_DuckDB_Value_Time_methods[] = {
	ZEND_ME(DuckDB_Value_Time, getTotalMicroseconds, arginfo_class_DuckDB_Value_Time_getTotalMicroseconds, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Time, getHour, arginfo_class_DuckDB_Value_Time_getHour, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Time, getMinutes, arginfo_class_DuckDB_Value_Time_getMinutes, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Time, getSeconds, arginfo_class_DuckDB_Value_Time_getSeconds, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Time, getOffset, arginfo_class_DuckDB_Value_Time_getOffset, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Time, getMicroseconds, arginfo_class_DuckDB_Value_Time_getMicroseconds, ZEND_ACC_PUBLIC)
	ZEND_ME(DuckDB_Value_Time, __toString, arginfo_class_DuckDB_Value_Time___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static void register_duckdb_symbols(int module_number)
{
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_OUT_OF_RANGE", DUCKDB_ERROR_OUT_OF_RANGE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_CONVERSION", DUCKDB_ERROR_CONVERSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_UNKNOWN_TYPE", DUCKDB_ERROR_UNKNOWN_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_DECIMAL", DUCKDB_ERROR_DECIMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_MISMATCH_TYPE", DUCKDB_ERROR_MISMATCH_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_DIVIDE_BY_ZERO", DUCKDB_ERROR_DIVIDE_BY_ZERO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_OBJECT_SIZE", DUCKDB_ERROR_OBJECT_SIZE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_INVALID_TYPE", DUCKDB_ERROR_INVALID_TYPE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_SERIALIZATION", DUCKDB_ERROR_SERIALIZATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_TRANSACTION", DUCKDB_ERROR_TRANSACTION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_NOT_IMPLEMENTED", DUCKDB_ERROR_NOT_IMPLEMENTED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_EXPRESSION", DUCKDB_ERROR_EXPRESSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_CATALOG", DUCKDB_ERROR_CATALOG, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_PARSER", DUCKDB_ERROR_PARSER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_PLANNER", DUCKDB_ERROR_PLANNER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_SCHEDULER", DUCKDB_ERROR_SCHEDULER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_EXECUTOR", DUCKDB_ERROR_EXECUTOR, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_CONSTRAINT", DUCKDB_ERROR_CONSTRAINT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_INDEX", DUCKDB_ERROR_INDEX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_STAT", DUCKDB_ERROR_STAT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_CONNECTION", DUCKDB_ERROR_CONNECTION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_SYNTAX", DUCKDB_ERROR_SYNTAX, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_SETTINGS", DUCKDB_ERROR_SETTINGS, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_BINDER", DUCKDB_ERROR_BINDER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_NETWORK", DUCKDB_ERROR_NETWORK, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_OPTIMIZER", DUCKDB_ERROR_OPTIMIZER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_NULL_POINTER", DUCKDB_ERROR_NULL_POINTER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_IO", DUCKDB_ERROR_IO, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_INTERRUPT", DUCKDB_ERROR_INTERRUPT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_FATAL", DUCKDB_ERROR_FATAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_INTERNAL", DUCKDB_ERROR_INTERNAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_INVALID_INPUT", DUCKDB_ERROR_INVALID_INPUT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_OUT_OF_MEMORY", DUCKDB_ERROR_OUT_OF_MEMORY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_PERMISSION", DUCKDB_ERROR_PERMISSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_PARAMETER_NOT_RESOLVED", DUCKDB_ERROR_PARAMETER_NOT_RESOLVED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_PARAMETER_NOT_ALLOWED", DUCKDB_ERROR_PARAMETER_NOT_ALLOWED, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_DEPENDENCY", DUCKDB_ERROR_DEPENDENCY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_HTTP", DUCKDB_ERROR_HTTP, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_MISSING_EXTENSION", DUCKDB_ERROR_MISSING_EXTENSION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_AUTOLOAD", DUCKDB_ERROR_AUTOLOAD, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\ERROR_SEQUENCE", DUCKDB_ERROR_SEQUENCE, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\INVALID_CONFIGURATION", DUCKDB_INVALID_CONFIGURATION, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\Value\\POSITIVE_INFINITY", PHP_DUCKDB_POSITIVE_INFINITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\Value\\NEGATIVE_INFINITY", PHP_DUCKDB_NEGATIVE_INFINITY, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("DuckDB\\Value\\FINITE", PHP_DUCKDB_FINITE, CONST_PERSISTENT);
}

static zend_class_entry *register_class_DuckDB_DuckDBException(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "DuckDBException", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_Exception, ZEND_ACC_NO_DYNAMIC_PROPERTIES);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry_Exception);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_ConnectionException(zend_class_entry *class_entry_DuckDB_DuckDBException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "ConnectionException", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_DuckDB_DuckDBException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DuckDB_DuckDBException);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_QueryException(zend_class_entry *class_entry_DuckDB_DuckDBException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "QueryException", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_DuckDB_DuckDBException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DuckDB_DuckDBException);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_AppendException(zend_class_entry *class_entry_DuckDB_DuckDBException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "AppendException", NULL);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_DuckDB_DuckDBException, ZEND_ACC_NO_DYNAMIC_PROPERTIES);
#else
	class_entry = zend_register_internal_class_ex(&ce, class_entry_DuckDB_DuckDBException);
	class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_DuckDB(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "DuckDB", class_DuckDB_DuckDB_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_Result(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "Result", class_DuckDB_Result_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_PreparedStatement(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "PreparedStatement", class_DuckDB_PreparedStatement_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_Appender(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB", "Appender", class_DuckDB_Appender_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_Value_Timestamp(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB\\Value", "Timestamp", class_DuckDB_Value_Timestamp_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_Value_Date(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB\\Value", "Date", class_DuckDB_Value_Date_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}

static zend_class_entry *register_class_DuckDB_Value_Time(zend_class_entry *class_entry_Stringable)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "DuckDB\\Value", "Time", class_DuckDB_Value_Time_methods);
#if (PHP_VERSION_ID >= 80400)
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_NOT_SERIALIZABLE);
#else
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	class_entry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
#endif
	zend_class_implements(class_entry, 1, class_entry_Stringable);

	return class_entry;
}
