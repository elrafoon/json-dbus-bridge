#!/usr/bin/env python2.5
#
# json dbus bridge
#
# Copyright (c) 2009 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

