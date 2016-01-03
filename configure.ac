AC_INIT([gmmproc5], [0.0.1], [qdlacz@gmail.com])
AC_PREREQ([2.68])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/mm/main/main.cc])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.12 check-news dist-xz no-dist-gzip silent-rules subdir-objects tar-pax no-define std-options -Wall])
AM_SILENT_RULES([yes])

AC_ARG_WITH(xml-library,
            [AS_HELP_STRING([--with-xml-library],
                            [which XML library to use (one of: pugi, libxml), default: auto - detects available XML libraries in following order of preference - pugi, libxml])],
            [XML_LIBRARY="${withval}"],
            [XML_LIBRARY=auto])

AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_AR
AC_LANG_PUSH([C++])
AX_CXX_COMPILE_STDCXX_14([noext], [mandatory])

m4_foreach([XML_LIB], [[pugi], [libxml]],
           [xml_lib_found=no
            AS_VAR_IF([xml_skip_rest],[],
                      [AS_IF([test "x${XML_LIBRARY}" = 'xauto' || test "x${XML_LIBRARY}" = "x""XML_LIB"],
                             [xml_lib_check=yes])
                       m4_case(XML_LIB,
                               [pugi],
                                 [AS_VAR_IF([xml_lib_check],[yes],
                                            [AC_CHECK_HEADER([pugixml.hpp],
                                                             [xml_lib_found=yes])])],
                               [libxml],
                                 [PKG_PROG_PKG_CONFIG
                                  PKG_CHECK_MODULES([LIBXML],[libxml-2.0],
                                                    [xml_lib_found=yes],
                                                    dnl override default
                                                    dnl not-found action, so
                                                    dnl configure won't abort.
                                                    [:])],
                               [])
                       AS_VAR_IF([xml_lib_found],[yes],
                                 [xml_skip_rest=yes])])
            m4_define([XML_LIB_UC], [m4_translit(XML_LIB,[a-z],[A-Z])])
            m4_define([XML_LIB_COND], [XML_]XML_LIB_UC)
            AM_CONDITIONAL(XML_LIB_COND,[test "x${xml_lib_found}" = 'xyes'])])

AC_LANG_POP([C++])

LT_INIT

AC_CONFIG_FILES([
  Makefile
])
AC_OUTPUT
