AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
	-I. \
	-I.. \
	-O0 -fno-short-enums \
	-DRPC_OFFSET=0

if WANT_DEBUG
AM_CPPFLAGS += -DDEBUG -DVERBOSE
endif

lib_LTLIBRARIES = librpc.la

rpcincludedir = $(includedir)/android-rpc
rpcinclude_HEADERS = \
	../rpc/clnt.h \
	../rpc/rpc.h \
	../rpc/svc.h \
	../rpc/xdr.h \
	../rpc/pmap_clnt.h \
	../rpc/rpc_router_ioctl.h \
	../rpc/types.h

librpc_la_SOURCES = \
	debug.h \
	clnt.c \
	ops.c \
	rpc.c \
	svc.c \
	svc_clnt_common.c \
	xdr.c

librpc_la_LIBADD = \
	-lpthread
