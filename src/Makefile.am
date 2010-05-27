bin_PROGRAMS = \
	json-dbus-bridge

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
    $(LIBEVENT_CPPFLAGS) \
	$(dbus_CFLAGS) \
	$(json_CFLAGS)

MAINTAINERCLEANFILES = \
	Makefile.in

noinst_HEADERS = \
	bridge_request.h \
	bridge.h

#
# binary
#
json_dbus_bridge_SOURCES = \
	json-dbus-bridge.c \
	bridge_request.c \
	bridge.c

json_dbus_bridge_LDADD = \
	-lfcgi \
	$(LIBEVENT_LIBS) \
	$(dbus_LIBS) \
	$(json_LIBS)

