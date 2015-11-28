AC_INIT([gmmproc5], [0.0.1], [qdlacz@gmail.com])
AC_PREREQ([2.68])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/mm/main/main.cc])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.12 check-news dist-xz no-dist-gzip silent-rules subdir-objects tar-pax no-define std-options -Wall])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_AR
AC_LANG_PUSH([C++])
dnl AX_CXX_COMPILE_STDCXX_11([noext])

AC_CHECK_HEADER([pugixml.hpp],
		dnl pugixml header found, alright
		[],
		[AC_MSG_ERROR([*** pugixml header not found])])

AC_LANG_POP([C++])

LT_INIT

AC_CONFIG_FILES([
  Makefile
])
AC_OUTPUT
