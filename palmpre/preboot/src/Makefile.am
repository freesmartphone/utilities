include $(top_srcdir)/Makefile.decl

NULL =

INCLUDES = \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

VALAC_ARGS = \
	--vapidir ../vapi \
	--pkg config \
	--pkg gio-2.0 \
	--pkg fsoframework-2.0 \
	--pkg evas \
	--pkg ecore \
	--pkg edje \
	--pkg gee-1.0 \
	--save-temps \
	--basedir $(top_srcdir)

BUILT_SOURCES = .vala.stamp

bin_PROGRAMS = preboot

preboot_VALASOURCES = \
	main.vala \
	$(NULL)

preboot_SOURCES = \
	$(preboot_VALASOURCES:.vala=.c) \
	$(NULL)

$(BUILT_SOURCES): $(preboot_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@

preboot_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST += \
	$(preboot_VALASOURCES) \
	$(BUILT_SOURCES) \
	$(NULL)

CLEANFILES = \
	*.c \
	*.h \
	$(BUILT_SOURCES) \
	$(NULL)

MAINTAINERCLEANFILES = \
	Makefile.in \
	$(NULL)
