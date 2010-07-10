AUTOMAKE_OPTIONS = subdir-objects
AM_DEFAULT_SOURCE_EXT = .cc

common_cxxflags = -Wall -Wextra -pedantic -Werror -std=c++0x

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

lib_LTLIBRARIES =

include $(top_srcdir)/src/src.am

.DELETE_ON_ERROR:
