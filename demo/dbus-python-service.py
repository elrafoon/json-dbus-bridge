#!/usr/bin/env python2.5
#
# json dbus bridge
#
# Copyright (c) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# To the extent possible under law, Pengutronix e.K. has waived all copyright
# and related or neighboring rights to JSON-DBus-Bridge Demo Code.
#

import gobject

import dbus
import dbus.service
import dbus.mainloop.glib

class Service(dbus.service.Object):
	def __init__(self, bus, name):
		self._state = 1
		dbus.service.Object.__init__(self, None, "/", name)

	@dbus.service.method("com.pengutronix.jdb.Data",
		in_signature='', out_signature='a{sai}')
	def Dump(self):
		return {"foo": [1, 2], "bar": [3, 4] }

if __name__ == '__main__':
	dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

	session_bus = dbus.SessionBus()
	name = dbus.service.BusName("com.pengutronix.jdb.demo1", session_bus)
	object = Service(session_bus, name)

	mainloop = gobject.MainLoop()
	mainloop.run()

