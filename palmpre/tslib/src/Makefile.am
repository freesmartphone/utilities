#
#  tslib/src/Makefile.am
#
#  Copyright (C) 2001 Russell King.
#
# This file is placed under the LGPL.  Please see the file
# COPYING for more details.
#
# $Id: Makefile.am,v 1.8 2005/02/28 23:44:12 kergoth Exp $
#

AM_CFLAGS	 = -DPLUGIN_DIR=\"@PLUGIN_DIR@\" -DTS_CONF=\"@TS_CONF@\" -DTS_POINTERCAL=\"@TS_POINTERCAL@\" \
		   $(DEBUGFLAGS) $(LIBFLAGS) $(VIS_CFLAGS)

include_HEADERS  = tslib.h

lib_LTLIBRARIES  = libts.la
libts_la_SOURCES = tslib.c

libts_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		   -release $(LT_RELEASE) -export-dynamic
libts_la_LIBADD  = -ldl -lhid
