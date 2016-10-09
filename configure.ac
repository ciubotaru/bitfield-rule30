#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(rule30, 0.6.0, vitalie@ciubotaru.tk)
AC_CONFIG_SRCDIR([rule30.c])
AC_ARG_WITH([local-bitfield], AS_HELP_STRING([--with-local-bitfield], [Ignore system bitfield library and build local]), [], [])
AC_CONFIG_HEADERS([config.h])

if test "x$prefix" = "xNONE" ; then
  prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE" ; then
  exec_prefix=$prefix
fi
PREFIX=$prefix
AC_SUBST([PREFIX])
AC_SUBST([exec_prefix])

# Checks for programs.
AC_PROG_CC([gcc pcc icc])
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB(m, main)
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB(rt, main)

# FIXME: Replace `main' with a function in `-lrule30':
#AC_CHECK_LIB(bitfield, main)
if test "x$with_local_bitfield" = "xyes" ; then
  AC_SUBST(HAVE_BITFIELD, 0)
else AC_SEARCH_LIBS([bfnew], [bitfield], [AC_SUBST(HAVE_BITFIELD, 1)], [AC_SUBST(HAVE_BITFIELD, 0)], [])
fi

# Checks for header files.
AC_CHECK_HEADERS(limits.h stdlib.h string.h bitfield.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

# AC_CONFIG_FILES(Makefile)
AC_OUTPUT([Makefile] [tests/Makefile] [examples/Makefile] [man/Makefile])
