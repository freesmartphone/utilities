AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/include \
-I$(top_srcdir)/mkbootimg \
-I$(top_builddir)/include \
-I$(top_builddir)/mkbootimg

bin_PROGRAMS = mkbootimg unbootimg

mkbootimg_SOURCES = \
mkbootimg.c
mkbootimg_LDADD = $(top_builddir)/libmincrypt/libmincrypt.la
mkbootimg_LDFLAGS = -static

unbootimg_SOURCES = \
unbootimg.c

EXTRA_DIST = bootimg.h

