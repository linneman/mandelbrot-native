#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.53])
AC_INIT([parmandel], [0.1], [linneman@gmx.de])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/rendering.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
PKG_PROG_PKG_CONFIG

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h string.h stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MEMCMP
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_PROG_LIBTOOL
AC_CHECK_FUNCS([memset])

AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([m], [cos])
PKG_CHECK_MODULES(sdl2, sdl2 >= 2.0.0 )

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
