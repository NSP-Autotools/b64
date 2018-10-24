ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = m4/gnulib-cache.m4
SUBDIRS = lib

bin_PROGRAMS = src/b64
src_b64_SOURCES = src/b64.c
src_b64_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib
src_b64_LDADD = lib/libgnu.a
