AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/include \
-I$(top_builddir)/include

bin_PROGRAMS = adb adbd

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

adbd_SOURCES = \
adb.c \
fdevent.c \
transport.c \
transport_local.c \
transport_usb.c \
sockets.c \
services.c \
file_sync_service.c \
jdwp_service.c \
framebuffer_service.c \
remount_service.c \
usb_linux_client.c \
log_service.c \
utils.c

adbd_LDADD = $(top_builddir)/libzipfile/libzipfile.la  \
                 $(top_builddir)/libcutils/libcutils.la 
adbd_CFLAGS = -DADB_HOST=0 -DHAVE_FORKEXEC -DANDROID_GADGET=1
adbd_LDFLAGS = -lpthread
