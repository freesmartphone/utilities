AC_INIT([preboot], [0.0.0], [morphis@gravedo.de], [preboot])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])
#m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

VALA_REQUIRED=0.7.10.3
GEE_REQUIRED=0.5.0
EINA_REQUIRED=0.0.0
EVAS_REQUIRED=0.0.0
ECORE_REQUIRED=0.0.0
ECORE_EVAS_REQUIRED=0.0.0
EDJE_REQUIRED=0.0.0
ELM_REQUIRED=0.0.0
GLIB_REQUIRED=2.18.0
DBUS_REQUIRED=1.2.1
DBUS_GLIB_REQUIRED=0.74
FSOBASICS_REQUIRED=0.9
FSOFRAMEWORK_REQUIRED=0.0.0
FSOGLIB_REQUIRED=0.0.0

PKG_CHECK_MODULES(VALA,
  vala-1.0 >= $VALA_REQUIRED)

PKG_CHECK_MODULES(DEPS,
  eina-0 >= $EINA_REQUIRED
  evas >= $EVAS_REQUIRED
  ecore >= $ECORE_REQUIRED
  ecore-evas >= $ECORE_EVAS_REQUIRED
  edje >= $EDJE_REQUIRED
  glib-2.0 >= $GLIB_REQUIRED
  gobject-2.0 >= $GLIB_REQUIRED
  fsobasics-2.0 >= $FSOBASICS_REQUIRED
  gee-1.0 >= $GEE_REQUIRED)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

AC_ARG_WITH(edje-cc,
[  --with-edje-cc=PATH	specify a specific path to edje_cc],
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)

# datadir
if test "x$prefix" != "xNONE"; then
  pkgdatadir='${prefix}/share/preboot'
else
  pkgdatadir='/usr/local/share/preboot'
fi
eval PACKAGE_DATADIR="${pkgdatadir}"
AC_DEFINE_UNQUOTED([PACKAGE_DATADIR], ["${PACKAGE_DATADIR}"], [Shared data directory])

AC_CONFIG_FILES([
  Makefile
  data/Makefile
  data/themes/Makefile
  conf/Makefile
  src/Makefile])

AC_OUTPUT
