bin_PROGRAMS = \
	json-dbus-bridge

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(dbus_CFLAGS) \
	$(json_CFLAGS)

MAINTAINERCLEANFILES = \
	Makefile.in

#
# binary
#
json_dbus_bridge_SOURCES = \
	json-dbus-bridge.c \
	bridge_request.c \
	bridge.c

json_dbus_bridge_LDADD = \
	-lfcgi \
	-levent \
	$(dbus_LIBS) \
	$(json_LIBS)

