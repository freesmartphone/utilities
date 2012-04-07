AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/include \
-I$(top_builddir)/include

bin_PROGRAMS = adb

adb_SOURCES = \
adb.c \
console.c \
transport.c \
transport_local.c \
transport_usb.c \
commandline.c \
adb_client.c \
sockets.c \
services.c \
fdevent.c \
file_sync_client.c \
usb_linux.c \
get_my_path_linux.c \
utils.c \
usb_vendors.c

adb_LDADD = $(top_builddir)/libzipfile/libzipfile.la  \
                 $(top_builddir)/libcutils/libcutils.la 
adb_CFLAGS = -DADB_HOST=1 -DHAVE_FORKEXEC
adb_LDFLAGS = -lpthread

