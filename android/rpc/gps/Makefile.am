AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
	-I. \
	-I.. \
	-O0 -fno-short-enums

bin_PROGRAMS = gps

gps_SOURCES = \
	debug.h \
	main.c

gps_LDADD = $(top_builddir)/librpc/librpc.la
gps_LDFLAGS =

