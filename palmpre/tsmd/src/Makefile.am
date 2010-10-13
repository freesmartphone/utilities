AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

NULL = 

AM_CPPFLAGS = \
	$(NULL)

AM_CFLAGS = \
	$(EXTRA_CFLAGS) \
	$(TSLIB_CFLAGS) \
	$(NULL)
if WANT_DEBUG
AM_CFLAGS += -DDEBUG
endif

bin_PROGRAMS = tsmd

tsmd_SOURCES = \
	tsmd.c \
	$(NULL)

tsmd_LDADD = $(TSLIB_LIBS)


