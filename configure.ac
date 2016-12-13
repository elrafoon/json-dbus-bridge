AC_PREREQ(2.59)

AC_INIT([json-dbus-bridge], 1.0.0, [bugs@pengutronix.de])
AC_CONFIG_SRCDIR([src/json-dbus-bridge.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

CFLAGS="${CFLAGS} -W -Wall"

#
# libtool library versioning stuff
#
# Library code modified:            REVISION++
# Interfaces changed/added/removed: CURRENT++   REVISION=0
# Interfaces added:                 AGE++
# Interfaces removed:               AGE=0
LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
LT_INIT()


#
# Checks for programs.
#
AC_PROG_CC

AM_INIT_AUTOMAKE([foreign no-exeext dist-bzip2])


#
# dbus bindings
#
REQUIRES_DBUS="dbus-1 >= 1.1.0"
AC_SUBST(REQUIRES_DBUS)
PKG_CHECK_MODULES(dbus, $REQUIRES_DBUS,)

##
# json
##
REQUIRES_JSON="json >= 0.8"
AC_SUBST(REQUIRES_JSON)
PKG_CHECK_MODULES(json, $REQUIRES_JSON)

#
# libevent2
#
AX_LIB_EVENT(2.0.0)
if test "x$ax_have_libevent" != "xyes"; then
	AC_MSG_ERROR([libevent not found.])
fi

#
# libfcgi
#
AC_CHECK_HEADER(fcgiapp.h,,AC_MSG_ERROR([libfcgi headers not found.]))
AC_CHECK_LIB(fcgi,FCGX_Init,,AC_MSG_ERROR([libfcgi not found.]))

#
# Debugging
#
AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug], [enable debugging @<:@default=yes@:>@]),
	[case "$enableval" in
	y | yes) CONFIG_DEBUG=yes ;;
        *) CONFIG_DEBUG=no ;;
    esac],
    [CONFIG_DEBUG=yes])
AC_MSG_RESULT([${CONFIG_DEBUG}])
if test "${CONFIG_DEBUG}" = "yes"; then
    CFLAGS="${CFLAGS} -Wsign-compare -Wfloat-equal -Wformat-security -g -O1"
    AC_DEFINE(DEBUG, 1, [debugging])
else
    CFLAGS="${CFLAGS} -O3"
fi
AC_SUBST(CONFIG_DEBUG)


AC_CONFIG_FILES([
	demo/dbus-simple-service/Makefile
	demo/lighttpd.conf
	demo/Makefile
	tests/lighttpd.conf
	tests/Makefile
	src/Makefile
	Makefile
])
AC_CONFIG_FILES([demo/run.sh],[chmod +x demo/run.sh])
AC_CONFIG_FILES([tests/run_test.sh],[chmod +x tests/run_test.sh])
AC_CONFIG_FILES([tests/test_finish.sh],[chmod +x tests/test_finish.sh])
AC_CONFIG_FILES([tests/test_init.sh],[chmod +x tests/test_init.sh])

AC_OUTPUT

