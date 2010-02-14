AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib

lib_LTLIBRARIES = libmincrypt.la

include_HEADERS = rsa.h sha.h

libmincrypt_la_SOURCES = \
rsa.h \
rsa.c \
sha.h \
sha.c 

libmincrypt_la_CFLAGS =
libmincrypt_la_LIBADD = 
libmincrypt_la_LDFLAGS = 
