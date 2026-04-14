dnl Autotools config.m4 for PHP extension duckdb

dnl Comments in this file start with the string 'dnl' (discard to next line).
dnl Remove where necessary.

dnl Otherwise use the '--enable-duckdb' configure option:
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

AS_VAR_IF([PHP_DUCKDB], [no],, [

  AC_DEFINE([HAVE_DUCKDB], [1],
    [Define to 1 if the PHP extension 'duckdb' is available.])

  dnl Candidate locations:
  dnl - bundled artifacts under ext/lib (CI)
  dnl - standard system prefixes (/opt/homebrew, /usr/local, /usr, /opt/local)
  DUCKDB_DIRS=""
  if test "$PHP_DUCKDB_DIR" != "no" && test "$PHP_DUCKDB_DIR" != "yes"; then
    DUCKDB_DIRS="$PHP_DUCKDB_DIR"
  else
    DUCKDB_DIRS="$ext_srcdir/lib /opt/homebrew /usr/local /usr /opt/local"
  fi

  DUCKDB_INCLUDE_DIR=""
  DUCKDB_LIB_DIR=""
  for DUCKDB_DIR in $DUCKDB_DIRS; do
    CANDIDATE_INCLUDE=""
    if test -f "$DUCKDB_DIR/duckdb.h"; then
      CANDIDATE_INCLUDE="$DUCKDB_DIR"
    elif test -f "$DUCKDB_DIR/include/duckdb.h"; then
      CANDIDATE_INCLUDE="$DUCKDB_DIR/include"
    fi

    CANDIDATE_LIB=""
    if test -f "$DUCKDB_DIR/libduckdb.a" || test -f "$DUCKDB_DIR/libduckdb.so" || test -f "$DUCKDB_DIR/libduckdb.dylib"; then
      CANDIDATE_LIB="$DUCKDB_DIR"
    elif test -f "$DUCKDB_DIR/lib/libduckdb.a" || test -f "$DUCKDB_DIR/lib/libduckdb.so" || test -f "$DUCKDB_DIR/lib/libduckdb.dylib"; then
      CANDIDATE_LIB="$DUCKDB_DIR/lib"
    fi

    if test -n "$CANDIDATE_INCLUDE" && test -n "$CANDIDATE_LIB"; then
      DUCKDB_INCLUDE_DIR="$CANDIDATE_INCLUDE"
      DUCKDB_LIB_DIR="$CANDIDATE_LIB"
      break
    fi
  done

  if test -z "$DUCKDB_INCLUDE_DIR"; then
    AC_MSG_ERROR([duckdb.h not found. Install libduckdb into a standard prefix (e.g. /usr/local or /opt/homebrew) or use --with-duckdb-dir.])
  fi

  if test -z "$DUCKDB_LIB_DIR"; then
    AC_MSG_ERROR([libduckdb not found. Install libduckdb into a standard prefix (e.g. /usr/local or /opt/homebrew) or use --with-duckdb-dir.])
  fi

  PHP_ADD_INCLUDE([$DUCKDB_INCLUDE_DIR])

  PHP_ADD_LIBRARY_WITH_PATH(duckdb, [$DUCKDB_LIB_DIR], DUCKDB_SHARED_LIBADD)
  PHP_SUBST(DUCKDB_SHARED_LIBADD)

  PHP_NEW_EXTENSION([duckdb],
    [duckdb.c duckdb_values.c],
    [$ext_shared],
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
