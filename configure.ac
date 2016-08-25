AC_INIT([gmmproc5], [0.0.1], [qdlacz@gmail.com])
AC_PREREQ([2.68])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([gmmproc/main/main.cc])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.12 check-news dist-xz no-dist-gzip silent-rules subdir-objects tar-pax no-define std-options -Wall])
AM_SILENT_RULES([yes])

AC_ARG_WITH(xml-library,
            [AS_HELP_STRING([--with-xml-library],
                            [which XML library to use (one of: pugi, libxml), default: auto - detects available XML libraries in the following order of preference - pugi, libxml])],
            [xml_library="${withval}"],
            [xml_library=auto])

AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_AR
PKG_PROG_PKG_CONFIG
AC_LANG_PUSH([C++])
AX_CXX_COMPILE_STDCXX_14([noext], [mandatory])

m4_foreach([XML_LIB], [[pugi], [libxml]],
           [xml_lib_found=no
            m4_case(XML_LIB,
                    [pugi],
                      [AC_CHECK_HEADER([pugixml.hpp],
                                       [xml_lib_found=yes])],
                    [libxml],
                      [PKG_CHECK_MODULES([LIBXML],[libxml-2.0],
                                         [xml_lib_found=yes],
                                         dnl override default
                                         dnl not-found action, so
                                         dnl configure won't abort.
                                         [:])],
                    [])
            AS_IF([test "x${xml_lib_found}" = 'xyes' && test "x${xml_library}" = 'xauto'],
                  [xml_library='XML_LIB'])
            AS_VAR_IF([xml_library],XML_LIB,
                      [xml_valid='yes'])
            m4_define([XML_LIB_UC], [m4_translit(XML_LIB,[a-z],[A-Z])])
            m4_define([XML_LIB_COND], [GMMPROC_XML_]XML_LIB_UC)
            m4_define([XML_LIB_USE_COND], [GMMPROC_XML_USE_]XML_LIB_UC)
            AM_CONDITIONAL(XML_LIB_COND,[test "x${xml_lib_found}" = 'xyes'])
            AM_CONDITIONAL(XML_LIB_USE_COND,[test "x${xml_library}" = 'x[]XML_LIB'])])
AS_VAR_IF([xml_valid],[yes],
          [:],
          [AC_MSG_ERROR([*** unknown xml library (${xml_library}) to use])])
AC_LANG_POP([C++])

AC_MSG_CHECKING([for gir directory])
PKG_CHECK_VAR([GIRDIR], [gobject-introspection-1.0], [girdir],
              [AC_MSG_RESULT([${GIRDIR}])],
              [AC_MSG_RESULT([not found])
               AC_MSG_ERROR([*** could not find the gir directory, do you have gobject-introspection-1.0 installed?])])
AC_SUBST([GIRDIR])

LT_INIT

AX_CODE_COVERAGE

AC_CONFIG_FILES([
  Makefile
])
AC_OUTPUT
