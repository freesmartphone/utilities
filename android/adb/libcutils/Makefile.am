AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/include \
-I$(top_builddir)/include

lib_LTLIBRARIES = libcutils.la

libcutils_la_SOURCES = \
array.c \
hashmap.c \
native_handle.c \
buffer.c \
socket_inaddr_any_server.c \
socket_local_client.c \
socket_local_server.c \
socket_loopback_client.c \
socket_loopback_server.c \
socket_network_client.c \
config_utils.c \
cpu_info.c \
load_file.c \
open_memstream.c \
strdup16to8.c \
strdup8to16.c \
record_stream.c \
process_name.c \
properties.c \
threads.c \
sched_policy.c \
iosched_policy.c \
ashmem-host.c \
abort_socket.c \
mspace.c \
selector.c \
tztime.c \
tzstrftime.c \
dlmalloc_stubs.c

libcutils_la_CFLAGS = -DHAVE_PTHREADS -DANDROID_SMP=0
libcutils_la_LIBADD = 
libcutils_la_LDFLAGS = 

docdir = $(datadir)/doc/cutils
doc_DATA = NOTICE
