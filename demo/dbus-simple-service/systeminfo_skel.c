/*
 *   json dbus bridge
 *
 *   Copyright (c) 2009 by Michael Olbrich <m.olbrich@pengutronix.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation; either version 2.1 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "systeminfo_skel.h"
#include "com_pengutronix_jdb_SystemInfo_xml.h"

static const char *introspect_system_info = \
DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE \
COM_PENGUTRONIX_JDB_SYSTEMINFO_XML \
;

#define DBUS_INTERFACE_SYSTEMINFO "com.pengutronix.jdb.SystemInfo"

DBusMessage *_system_info_error(DBusMessage *message, int err)
{
	return dbus_message_new_error_printf(message, DBUS_ERROR_FAILED,
		"Method call failed (%d): %s", err, strerror(err));
}

int _system_info_date(system_info_t *self, unsigned int *date)
{
	if (!self->Date)
		return EAGAIN;
	return self->Date(self, date);
}

int _system_info_noop(system_info_t *self)
{
	if (!self->Noop)
		return EAGAIN;
	return self->Noop(self);
}

int _system_info_ping(system_info_t *self, const char *ping, const char **pong)
{
	if (!self->Ping)
		return EAGAIN;
	return self->Ping(self, ping, pong);
}

DBusHandlerResult _system_info_dispatch(DBusConnection *c, DBusMessage *message,
					void *data)
{
	system_info_t *self = (system_info_t*)data;
	DBusMessage *reply;

	if (dbus_message_is_method_call(message, DBUS_INTERFACE_INTROSPECTABLE,
			"Introspect")) {

		reply = dbus_message_new_method_return(message);

		dbus_message_append_args(reply,
			DBUS_TYPE_STRING, &introspect_system_info,
			DBUS_TYPE_INVALID);

		dbus_connection_send(c, reply, 0);

		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	if (dbus_message_is_method_call(message, DBUS_INTERFACE_SYSTEMINFO,
			"Ping")) {

		int ret;
		const char *s1, *s2;

		if (!dbus_message_get_args(message, 0,
		                           DBUS_TYPE_STRING, &s1,
		                           DBUS_TYPE_INVALID)) {
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}

		if ((ret = _system_info_ping(self, s1, &s2)) != 0) {
			reply = _system_info_error(message, ret);
		}
		else {
			reply = dbus_message_new_method_return(message);
			dbus_message_append_args(reply,
						 DBUS_TYPE_STRING, &s2,
						 DBUS_TYPE_INVALID);
		}
		dbus_connection_send(c, reply, 0);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	if (dbus_message_is_method_call(message, DBUS_INTERFACE_SYSTEMINFO,
			"Noop")) {

		int ret;

		if ((ret = _system_info_noop(self)) != 0) {
			reply = _system_info_error(message, ret);
		}
		else {
			reply = dbus_message_new_method_return(message);
		}
		dbus_connection_send(c, reply, 0);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	if (dbus_message_is_method_call(message, DBUS_INTERFACE_SYSTEMINFO,
			"Date")) {

		int ret;
		unsigned int u1;

		if ((ret = _system_info_date(self, &u1)) != 0) {
			reply = _system_info_error(message, ret);
		}
		else {
			reply = dbus_message_new_method_return(message);
			dbus_message_append_args(reply,
						 DBUS_TYPE_UINT32, &u1,
						 DBUS_TYPE_INVALID);
		}
		dbus_connection_send(c, reply, 0);
		dbus_message_unref(reply);
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int system_info_init(system_info_t *self)
{
	self->connection = NULL;
	self->path = NULL;
	self->Date = NULL;
	self->Noop = NULL;
	self->Ping = NULL;
	return 0;
}

int system_info_destroy(system_info_t *self)
{
	dbus_connection_unregister_object_path(self->connection, self->path);
	free(self->path);
	return 0;
}

int system_info_register(system_info_t *self, DBusConnection *c, const char *path)
{
	DBusObjectPathVTable vtable;

	vtable.unregister_function = 0;
	vtable.message_function = _system_info_dispatch;

	if (!dbus_connection_register_object_path(c, path, &vtable, self))
		return ENOMEM;

	self->connection = c;
	self->path = strdup(path);

	return 0;
}


