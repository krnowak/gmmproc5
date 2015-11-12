AUTOMAKE_OPTIONS = subdir-objects
AM_DEFAULT_SOURCE_EXT = .cc

common_cxxflags = -Wall -Wextra -Wpedantic -Werror -std=c++14
common_cppflags = -I$(top_srcdir)/src

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

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
	rm -rf $(gmmproc5docdir)

noinst_LTLIBRARIES =
bin_PROGRAMS =

include $(top_srcdir)/src/src.am

.DELETE_ON_ERROR:
