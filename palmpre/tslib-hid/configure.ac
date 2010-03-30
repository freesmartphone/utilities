#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(tslib, 1.0.0, kergoth@handhelds.org)
# AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(dist-bzip2)
AM_CONFIG_HEADER(config.h)

PACKAGE_DESCRIPTION="Touchscreen Access Library"
AC_SUBST(PACKAGE_DESCRIPTION)

# Set our default flags variables
TS_DEFAULT_FLAGS

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
TS_CC_HIDDEN_VISIBILITY
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_DISABLE_STATIC
AC_ENABLE_SHARED
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([dl], [dlopen])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MMAP
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gettimeofday memmove memset munmap select strcasecmp strchr strdup strtoul])

AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE(debug,
	AS_HELP_STRING(--enable-debug
		Enable debug messages from filters (default=no)), 
	,
	[enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "$enable_debug" = "yes"; then
	DEBUGFLAGS="-g -ggdb -DDEBUG"
else
	DEBUGFLAGS=""
fi
AC_SUBST(DEBUGFLAGS)

LIBFLAGS="-DTSLIB_INTERNAL"
AC_SUBST(LIBFLAGS)

TS_CONF='${sysconfdir}/ts.conf'
AC_SUBST(TS_CONF)

TS_POINTERCAL='${sysconfdir}/pointercal'
AC_SUBST(TS_POINTERCAL)

# Library versioning
LT_RELEASE=1.0
LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tslib.pc:tslib.pc.in
                 tslib-$LT_RELEASE.pc:tslib.pc.in])
AC_OUTPUT
