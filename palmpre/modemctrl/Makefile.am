NULL =

AUTOMAKE_OPTIONS = subdir-objects
VALAC_ARGS = \
	--basedir $(top_srcdir) \
	--vapidir vapi \
	--pkg posix \
	--pkg linux \
	--pkg gio-2.0 \
	--pkg fsobasics-2.0 \
	--save-temps \
	--basedir $(top_srcdir)

SUBDIRS = \
	$(NULL)

INCLUDES = \
	-I$(top_srcdir) \
	$(DEPS_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = src/.vala.stamp

sbin_PROGRAMS = modemctrl

modemctrl_VALASOURCES = \
	src/main.vala \
	$(NULL)

modemctrl_SOURCES = \
	$(modemctrl_VALASOURCES:.vala=.c) \
	$(NULL)

src/.vala.stamp: $(modemctrl_VALASOURCES)
	$(VALAC) -C $(VALAC_ARGS) $^
	touch $@

modemctrl_LDADD = \
	$(DEPS_LIBS) \
	$(NULL)

EXTRA_DIST = \
	MAINTAINERS \
	$(modemctrl_VALASOURCES) \
	$(BUILT_SOURCES) \
	$(NULL)

CLEANFILES = \
	src/*.? \
	src/*.l? \
	$(BUILT_SOURCES) \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES = aclocal.m4 compile config.guess config.sub \
                       configure depcomp install-sh ltmain.sh     \
                       Makefile.in missing config.h.in            \
                       intltool-extract \
                       intltool-merge \
                       intltool-update \
                       mkinstalldirs \
                       .deps \
                       src/*.c src/*.h \
                       $(NULL)
