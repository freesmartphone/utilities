# include $(top_srcdir)/Makefile.decl

NULL =

VALAC_ARGS = \
	--basedir $(top_srcdir) \
	--vapidir ../vapi \
	--pkg dbus-glib-1 \
	--save-temps

if WANT_DEBUG
VALAC_ARGS += -D DEBUG
endif

INCLUDES = \
	$(DBUS_CFLAGS) \
	$(GLIB_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = .vala.stamp

ANDROID_RPC_LIBS = \
	../librpc/librpc.la \
	$(NULL)

bin_PROGRAMS = fso-msm-gps-activator

fso_msm_gps_activator_VALASOURCES = \
	main.vala \
	$(NULL)

fso_msm_gps_activator_SOURCES = \
	$(fso_msm_gps_activator_VALASOURCES:.vala=.c) \
	gps.c \
	$(NULL)

$(BUILT_SOURCES): $(fso_msm_gps_activator_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@

fso_msm_gps_activator_LDADD = \
	$(top_srcdir)/librpc/librpc.la \
	$(ANDROID_RPC_LIBS) \
	$(DBUS_LIBS) \
	$(GLIB_LIBS) \
	$(NULL)

EXTRA_DIST = \
	$(fso_msm_gps_activator_VALASOURCES) \
	$(BUILT_SOURCES) \
	$(NULL)

CLEANFILES = \
	$(BUILT_SOURCES) \
	$(NULL)

MAINTAINERCLEANFILES = \
	Makefile.in \
	$(NULL)
