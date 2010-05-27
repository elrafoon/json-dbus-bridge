
noinst_PROGRAMS = \
	dbus-simple-service

AM_CPPFLAGS = \
	$(dbus_CFLAGS)

EXTRA_DIST = \
	com_pengutronix_jdb_SystemInfo.xml

MAINTAINERCLEANFILES = \
	Makefile.in

CLEANFILES = \
	com_pengutronix_jdb_SystemInfo_xml.h

noinst_HEADERS = \
	systeminfo_skel.h

%_xml.h: %.xml
	echo "#define $(shell echo $@ | tr [a-z] [A-Z] | sed -e "s,-,_,g" | sed -e "s,.H,,g") \\" > $@
	cat $< | \
	  sed -e 's,",\\\",g' | \
	  sed -e 's,^,",g'    | \
	  sed -e 's,$$,\\n" \\,g' \
	  >> $@; \
	echo "\"\"" >> $@

#
# binary
#
dbus_simple_service_SOURCES = \
	dbus-simple-service.c \
	systeminfo_skel.c

$(srcdir)/systeminfo_skel.c: com_pengutronix_jdb_SystemInfo_xml.h

dbus_simple_service_LDADD = \
	$(dbus_LIBS)

