AC_PREREQ(2.53)
AC_INIT([tsmd], [1.0.0], [vfebvre@easter-eggs.com], [tsmd])
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_HEADERS(config.h)
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES] , [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_C_O
AC_GNU_SOURCE
AC_ISC_POSIX
AC_DISABLE_STATIC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(TSLIB, 
                 tslib)
AC_SUBST(TSLIB_CFLAGS)
AC_SUBST(TSLIB_LIBS)

# check for debugging
AC_ARG_ENABLE(debug,
  [  --enable-debug            Enable debug build (default=disabled)],
  [debug=$enableval],
  [debug="no"])
AM_CONDITIONAL( [WANT_DEBUG], [test x"$debug" = x"yes"])


# output stuff
AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT
