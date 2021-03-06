AC_INIT([cmtspeechtest], [0.0.0], [smartphones-userland@linuxtogo.org], [cmtspeechtest])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_C_O
AC_GNU_SOURCE
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

VALA_REQUIRED=0.12.0
AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)
m4_ifdef([AM_PROG_VALAC], [AM_PROG_VALAC([$VALA_REQUIRED])])

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

# requirements
GLIB_REQUIRED=2.26.1
FSO_GLIB_REQUIRED=2011.04.12
CMTSPEECHDATA_REQUIRED=2.1.1

# glib and friends
PKG_CHECK_MODULES(GLIB,
  glib-2.0 >= $GLIB_REQUIRED
  gobject-2.0 >= $GLIB_REQUIRED
  gio-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

# cmtspeechdata
PKG_CHECK_MODULES(CMTSPEECHDATA,
  libcmtspeechdata >= $CMTSPEECHDATA_REQUIRED)
AC_SUBST(CMTSPEECHDATA_CFLAGS)
AC_SUBST(CMTSPEECHDATA_LIBS)

# fso-glib
PKG_CHECK_MODULES(FSO_GLIB,
  fso-glib-1.0 >= $FSO_GLIB_REQUIRED)
AC_SUBST(FSO_GLIB_CFLAGS)
AC_SUBST(FSO_GLIB_LIBS)

# check for debugging
AC_ARG_ENABLE(debug,
  [  --enable-debug            Enable debug build (default=disabled)],
  [debug=$enableval],
  [debug="no"])
AM_CONDITIONAL( [WANT_DEBUG], [test x"$debug" = x"yes"])

# output
AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT

# info
echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE_NAME $PACKAGE_VERSION-$GITV"
echo "------------------------------------------------------------------------"
echo
echo "Configuration Options:"
echo
echo "  debug build.............: $debug"
echo
echo "  prefix..................: $prefix"
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' to compile and 'make install' to install this package."
