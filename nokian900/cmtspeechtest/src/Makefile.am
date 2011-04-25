include $(top_srcdir)/Makefile.decl

NULL =

VALAC_ARGS = \
	--basedir $(top_srcdir) \
	--vapidir ../vapi \
	--pkg posix \
	--pkg libcmtspeechdata \
	--pkg fso-glib-1.0 \
	--save-temps

if WANT_DEBUG
VALAC_ARGS += -D DEBUG
endif

INCLUDES = \
	$(GLIB_CFLAGS) \
	$(FSO_GLIB_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = cmtspeechtest

cmtspeechtest_VALASOURCES = \
	main.vala \
	$(NULL)

cmtspeechtest_SOURCES = \
	$(cmtspeechtest_VALASOURCES:.vala=.c) \
	$(NULL)

$(BUILT_SOURCES): $(cmtspeechtest_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@

cmtspeechtest_LDADD = \
	$(CMTSPEECHDATA_LIBS) \
	$(FSO_GLIB_LIBS) \
	$(GLIB_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(cmtspeechtest_VALASOURCES) \
	$(BUILT_SOURCES) \
	$(man_MANS) \
	$(NULL)

CLEANFILES = \
	*.c \
	*.h \
	$(BUILT_SOURCES) \
	$(NULL)

MAINTAINERCLEANFILES = \
	Makefile.in \
	$(NULL)
