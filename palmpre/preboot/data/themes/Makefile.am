MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = default.edj

CLEANFILES = default.edj

themedir = $(pkgdatadir)/themes/
theme_DATA = default.edj

all: default.edj
default.edj: default/default.edc
	edje_cc -v -id default/images -fd default/fonts default/default.edc default.edj
