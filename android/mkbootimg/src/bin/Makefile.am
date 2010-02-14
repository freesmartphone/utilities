AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/bin 

bin_PROGRAMS = mkbootimg

include_HEADERS = bootimg.h

mkbootimg_SOURCES = \
bootimg.h \
mkbootimg.c 

mkbootimg_LDADD = $(top_builddir)/src/lib/libmincrypt.la
mkbootimg_LDFLAGS =

