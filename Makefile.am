AM_DEFAULT_SOURCE_EXT = .cc

common_cxxflags = -Wall -Wextra -Wpedantic -Werror
common_cppflags = -I$(top_srcdir)/src $(xml_base_impl_cppflags)

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

gmmproc5docdir = ${docdir}/doc/gmmproc5
dist_gmmproc5doc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS

noinst_LIBRARIES =
bin_PROGRAMS =
noinst_PROGRAMS =
BUILT_SOURCES =

include $(top_srcdir)/src/src.am

.DELETE_ON_ERROR:
