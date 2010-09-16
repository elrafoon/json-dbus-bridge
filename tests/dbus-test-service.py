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

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='y', out_signature='y')
	def EchoBYTE(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='b', out_signature='b')
	def EchoBOOLEAN(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='n', out_signature='n')
	def EchoINT16(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='q', out_signature='q')
	def EchoUINT16(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='i', out_signature='i')
	def EchoINT32(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='u', out_signature='u')
	def EchoUINT32(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='x', out_signature='x')
	def EchoINT64(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='t', out_signature='t')
	def EchoUINT64(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='d', out_signature='d')
	def EchoDOUBLE(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='s', out_signature='s')
	def EchoSTRING(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Basic",
		in_signature='ii', out_signature='i')
	def SumINT32(self, val1, val2):
		return val1 + val2;

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='v', out_signature='v')
	def EchoVariant(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='av', out_signature='av')
	def EchoVariantArray(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='ai', out_signature='ai')
	def EchoArray(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='ai', out_signature='i')
	def CountArray(self, val):
		return len(val);

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='a{si}', out_signature='a{si}')
	def EchoDict(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='a{si}', out_signature='as')
	def DictKeys(self, val):
		keys = val.keys()
		keys.sort()
		return keys
	
	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='(aiuaiu)', out_signature='(aiuaiu)')
	def EchoStruct(self, val):
		return val;

	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='a(aiu)', out_signature='a(aiu)')
	def EchoStructArray(self, val):
		return val;
	
	@dbus.service.method("com.pengutronix.jdb.Test.Complex",
		in_signature='a(aiu)', out_signature='i')
	def CountStructIntegerValues(self, val):
		sum = 0
		for struct in val:
			ai = struct[0]
			u  = struct[1]
			for i in ai:
				sum += i
			sum += u
		return sum;

if __name__ == '__main__':
	dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

	session_bus = dbus.SessionBus()
	name = dbus.service.BusName("com.pengutronix.jdb.test", session_bus)
	object = Service(session_bus, name)

	mainloop = gobject.MainLoop()
	mainloop.run()


