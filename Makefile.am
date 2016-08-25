AM_DEFAULT_SOURCE_EXT = .cc

common_cxxflags = -Wall -Wextra -Wpedantic -Werror
common_cppflags =

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
check_PROGRAMS =
TESTS =

include $(top_srcdir)/gmmproc/gmmproc.am
include $(top_srcdir)/ext/ext.am

.DELETE_ON_ERROR:

update-m4:
	for f in ax_code_coverage.m4 ax_cxx_compile_stdcxx.m4 ax_cxx_compile_stdcxx_14.m4; do \
	  wget -O "m4/$${f}" "http://git.savannah.gnu.org/gitweb/?p=autoconf-archive.git;a=blob_plain;f=m4/$${f}"; \
	done;

CODE_COVERAGE_IGNORE_PATTERN = "/usr/*"
@CODE_COVERAGE_RULES@
