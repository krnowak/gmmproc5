AUTOMAKE_OPTIONS = subdir-objects
AM_DEFAULT_SOURCE_EXT = .cc

common_cxxflags = -Wall -Wextra -pedantic -Werror

ACLOCAL_AMFLAGS = -I build ${ACLOCAL_FLAGS}

gmmproc5docdir = ${docdir}/doc/gmmproc5
gmmproc5doc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(gmmproc5doc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(gmmproc5docdir)

lib_LTLIBRARIES = \
  src/common/libcommon.la \
	src/api/libapi.la \
	src/apiparsers/defsparser/libdefsparser.la

#
# libcommon
#
include $(top_srcdir)/src/common/filelist.am

src_common_libcommon_la_SOURCES = $(libcommon_src)

libcommon_la_CXXFLAGS = $(common_cxxflags)

#
# libapi
#
include $(top_srcdir)/src/api/filelist.am

src_api_libapi_la_SOURCES = $(libapi_src)

src_api_libapi_la_CPPFLAGS = \
	-Isrc/common

src_api_libapi_la_CXXFLAGS = $(common_cxxflags)

#
# libdefsparser
#
include $(top_srcdir)/src/apiparsers/defsparser/filelist.am

src_apiparsers_defsparser_libdefsparser_la_SOURCES = $(libdefsparser_src)

src_apiparsers_defsparser_libdefsparser_la_CPPFLAGS = \
	-Isrc/api

src_apiparsers_defsparser_libdefsparser_la_CXXFLAGS = $(common_cxxflags)

.DELETE_ON_ERROR:
