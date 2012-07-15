AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in
NULL =

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(NULL)

bin_PROGRAMS = alsa-initializer

alsa_initializer_SOURCES = main.vala
alsa_initializer_VALAFLAGS = \
	--basedir $(top_srcdir) \
	--pkg alsa \
	--pkg glib-2.0 \
	$(NULL)
alsa_initializer_CFLAGS = \
	$(ALSA_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(NULL)
alsa_initializer_LDADD = \
	$(ALSA_LIBS) \
	$(GLIB_LIBS) \
	$(NULL)
