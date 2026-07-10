dnl Autotools config.m4 for PHP extension duckdb

PHP_ARG_ENABLE([duckdb],
  [whether to enable duckdb support],
  [AS_HELP_STRING([--enable-duckdb],
    [Enable duckdb support])],
  [no])

PHP_ARG_WITH([duckdb-dir],
  [DuckDB installation prefix],
  [AS_HELP_STRING([--with-duckdb-dir=DIR],
    [DuckDB installation prefix (contains include/duckdb.h and lib/libduckdb.*)])],
  [no],
  [no])

if test "$PHP_DUCKDB" != "no"; then
  AC_MSG_CHECKING(PHP version is at least 8.1.0)

  if test -z "$PHP_VERSION_ID"; then
    if test -z "$PHP_CONFIG"; then
      AC_MSG_ERROR(php-config not found)
    fi
    PHP_VERNUM=`$PHP_CONFIG --vernum`
  else
    PHP_VERNUM="$PHP_VERSION_ID"
  fi

  if test $PHP_VERNUM -lt 80100; then
    AC_MSG_ERROR(PHP Version is below required 8.1.0)
  fi

  if test "$PHP_DUCKDB_DIR" != "no"; then
    if test -r "$PHP_DUCKDB_DIR/include/duckdb.h"; then
      DUCKDB_DIR="$PHP_DUCKDB_DIR"
    fi
  else
    for dir in /opt/homebrew /usr/local /usr; do
      AC_MSG_RESULT(checking in $dir)
      if test -r "$dir/include/duckdb.h"; then
        DUCKDB_DIR=$dir
        AC_MSG_RESULT(found in $dir)
        break
      fi
    done
  fi

  if test -z "$DUCKDB_DIR"; then
    AC_MSG_ERROR(Cannot find libduckdb library)
  fi

  PHP_ADD_INCLUDE($DUCKDB_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(duckdb, $DUCKDB_DIR/lib, DUCKDB_SHARED_LIBADD)
  PHP_SUBST(DUCKDB_SHARED_LIBADD)
  PHP_NEW_EXTENSION(duckdb, duckdb.c duckdb_values.c, $ext_shared, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
