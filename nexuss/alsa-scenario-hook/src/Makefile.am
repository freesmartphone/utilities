include $(top_srcdir)/Makefile.decl
NULL =

AM_CFLAGS = \
	-include $(CONFIG_HEADER) \
	-DG_LOG_DOMAIN=\"alsa-scenario-hook\" \
	$(NULL)
AM_VALAFLAGS =

modlibexecdir = $(libdir)/alsa-lib
modlibexec_LTLIBRARIES = alsa_scenario_hook.la
alsa_scenario_hook_la_SOURCES = \
	plugin.vala \
	alsa-wrapper.c \
	$(NULL)
alsa_scenario_hook_la_VALAFLAGS = \
	--basedir $(top_srcdir) \
	--pkg alsa \
	--pkg glib-2.0 \
	--pkg gio-2.0 \
	$(NULL)
alsa_scenario_hook_la_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(ALSA_CFLAGS) \
	$(NULL)
alsa_scenario_hook_la_LIBADD = \
	$(GLIB_LIBS) \
	$(ALSA_LIBS) \
	$(NULL)
alsa_scenario_hook_la_LDFLAGS = -no-undefined -module -avoid-version
