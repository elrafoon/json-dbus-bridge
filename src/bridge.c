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
#include <stdio.h>
#include <stdlib.h>

#include <dbus/dbus.h>

#include <event.h>

#include "bridge.h"

void _bridge_wkaeup_main(void *data)
{
	bridge_t *self = (bridge_t*)data;

	event_base_loopbreak(self->event_base);
}

void _bridge_dispatch_status(DBusConnection *connection, DBusDispatchStatus new_status, void *data)
{
	bridge_t *self = (bridge_t*)data;
	(void)connection;
	(void)new_status;

	event_base_loopbreak(self->event_base);
}

void _bridge_handle_dbus_watch(evutil_socket_t s, short flags, void *data)
{
	DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, 0);
	DBusWatch *watch = (DBusWatch*)data;
	int f = 0;

	(void)s;

	if (flags & EV_READ)
		f |= DBUS_WATCH_READABLE;
	if (flags & EV_WRITE)
		f |= DBUS_WATCH_WRITABLE;
	dbus_watch_handle(watch, f);
	while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS);
}

void _bridge_handle_dbus_timeout(evutil_socket_t s, short flags, void *data)
{
	DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, 0);
	DBusTimeout *timeout = (DBusTimeout*)data;

	(void)s;

	if (!(flags & EV_TIMEOUT))
		return;

	dbus_timeout_handle(timeout);
	while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS);
}

void _bridge_toggle_watch(DBusWatch *watch, void *data)
{
	struct event *ev = (struct event*)dbus_watch_get_data(watch);
	(void)data;

	if (dbus_watch_get_enabled(watch))
		event_add(ev, 0);
	else
		event_del(ev);
	return;
}

dbus_bool_t _bridge_add_watch(DBusWatch *watch, void *data)
{
	bridge_t *self = (bridge_t*)data;
	struct event *ev;

	int f = 0;
	if (dbus_watch_get_flags(watch) & DBUS_WATCH_READABLE)
		f |= EV_READ;
	if (dbus_watch_get_flags(watch) & DBUS_WATCH_WRITABLE)
		f |= EV_WRITE;

	ev = event_new(self->event_base, dbus_watch_get_unix_fd(watch),
		f|EV_PERSIST, _bridge_handle_dbus_watch, watch);

	dbus_watch_set_data(watch, ev, 0);
	_bridge_toggle_watch(watch, data);
	return TRUE;
}

void _bridge_remove_watch(DBusWatch *watch, void *data)
{
	struct event *ev = (struct event*)dbus_watch_get_data(watch);
	(void)data;

	event_del(ev);
	event_free(ev);
	return;
}

void _bridge_toggle_timeout(DBusTimeout *timeout, void *data)
{
	struct event *ev = (struct event*)dbus_timeout_get_data(timeout);
	int interval = dbus_timeout_get_interval(timeout);
	struct timeval t = {
		.tv_sec = interval/1000,
		.tv_usec = interval%1000
	};
	(void)data;

	if (dbus_timeout_get_enabled(timeout))
		event_add(ev, &t);
	else
		event_del(ev);
	return;
}

dbus_bool_t _bridge_add_timeout(DBusTimeout *timeout, void *data)
{
	bridge_t *self = (bridge_t*)data;
	struct event *ev;

	ev = event_new(self->event_base, -1,
		EV_PERSIST, _bridge_handle_dbus_timeout, timeout);
	dbus_timeout_set_data(timeout, ev, 0);
	_bridge_toggle_timeout(timeout, data);
	return TRUE;
}

void _bridge_remove_timeout(DBusTimeout *timeout, void *data)
{
	struct event *ev = (struct event*)dbus_timeout_get_data(timeout);
	(void)data;

	event_del(ev);
	event_free(ev);
	return;
}



void bridge_handle_cgi(evutil_socket_t s, short flags, void *data)
{
	bridge_t *self = (bridge_t*)data;
	bridge_request_t *request;
	int ret;
	(void)s;

	if (!(flags & EV_READ))
		return;

	if (!self->head) {
		fprintf(stderr, "%s malloc\n", __FUNCTION__);
		self->head = malloc(sizeof(bridge_request_t));
		ret = bridge_request_init(self->head, self,
			self->dbus_connection, self->socket);
		if (ret != 0) {
			fprintf(stdout, "bridge_request_init failed: %d", ret);
			return;
		}
	}
	request = self->head;

	if ((ret = bridge_request_accept(request)) != 0)
		return;
	if ((ret = bridge_request_handle(request)) != 0)
		return;

	self->head = request->next;
	request->next = 0;
}

int bridge_init(bridge_t *self, const char *socket_path)
{
	DBusError dbus_error;

	if (FCGX_Init() != 0) {
		fprintf(stdout, "FCGX_Init failed.");
		return EINVAL;
	}

	if (socket_path) {
		if ((self->socket = FCGX_OpenSocket(socket_path, SOCKET_BACKLOG)) < 0) {
			fprintf(stdout, "FCGX_OpenSocket failed: \"%s\".",
					   socket_path);
			return EINVAL;
		}
	}
	else
		self->socket = 0;


	/* attach us to dbus */
	dbus_error_init(&dbus_error);
	if ((self->dbus_connection = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error)) == 0) {
		fprintf(stdout, "couldn't connect to dbus: %s: %s", dbus_error.name, dbus_error.message);
		return EINVAL;
	}

	self->head = 0;

	self->event_base = event_base_new();

	struct event * ev = event_new(self->event_base, self->socket, EV_READ|EV_PERSIST, bridge_handle_cgi, self);
	event_add(ev, 0);

	dbus_connection_set_watch_functions(self->dbus_connection,
		_bridge_add_watch, _bridge_remove_watch, _bridge_toggle_watch,
		self, 0);

	dbus_connection_set_timeout_functions(self->dbus_connection,
		_bridge_add_timeout, _bridge_remove_timeout, _bridge_toggle_timeout,
		self, 0);

	dbus_connection_set_dispatch_status_function(self->dbus_connection,
		_bridge_dispatch_status, self, 0);

	dbus_connection_set_wakeup_main_function(self->dbus_connection,
		_bridge_wkaeup_main, self, 0);

	return 0;
}

int bridge_destroy(bridge_t *self)
{
	bridge_request_t *request, *next;
	for (request = self->head; request; request = next) {
		next = request->next;
		bridge_request_destroy(request);
	}
	return 0;
}

int bridge_run(bridge_t *self)
{

	while (1) {
		event_base_loop(self->event_base, 0);
		while (dbus_connection_dispatch(self->dbus_connection) == DBUS_DISPATCH_DATA_REMAINS);
	}

/*
	while(TRUE) {
		if (!self->head) {
			self->head = malloc(sizeof(bridge_request_t));
			ret = bridge_request_init(self->head, self,
				self->dbus_connection, self->socket);
		}
		if (ret != 0) {
			fprintf(stdout, "bridge_request_init failed: %d", ret);
			return ret;
		}
		request = self->head;
		self->head = request->next;
		request->next = 0;

		if ((ret = bridge_request_accept(request)) != 0)
			return ret;
		if ((ret = bridge_request_handle(request)) != 0)
			return ret;
	}
*/
	return 0;
}

