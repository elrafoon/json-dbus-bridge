/*
 * json dbus bridge
 *
 * Copyright (c) 2009, 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <dbus/dbus.h>

#include <event.h>

#include "bridge.h"

#if CONFIG_SYSTEMD
# include <systemd/sd-daemon.h>
#endif

#define container_of(ptr, type, member) ({ \
		                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
		                (type *)( (char *)__mptr - offsetof(type,member) );})

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
	struct bridge_watch_data *cb_data = (struct bridge_watch_data *)data;
	DBusConnection *connection = dbus_bus_get(cb_data->self->dbus_bus_type, 0);
	unsigned int f = 0;

	(void)s;

	if (flags & EV_READ)
		f |= DBUS_WATCH_READABLE;
	if (flags & EV_WRITE)
		f |= DBUS_WATCH_WRITABLE;
	if (!dbus_watch_handle(cb_data->watch, f))
		return;
	while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS);
}

void _bridge_handle_dbus_timeout(evutil_socket_t s, short flags, void *data)
{
	struct bridge_timeout_data *cb_data = (struct bridge_timeout_data *)data;
	DBusConnection *connection = dbus_bus_get(cb_data->self->dbus_bus_type, 0);

	(void)s;

	if (!(flags & EV_TIMEOUT))
		return;

	if (!dbus_timeout_handle(cb_data->timeout))
		return;
	while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS);
}

void _bridge_toggle_watch(DBusWatch *watch, void *data)
{
	struct bridge_watch_data *cb_data = (struct bridge_watch_data *)dbus_watch_get_data(watch);
	(void)data;

	if (dbus_watch_get_enabled(watch))
		event_add(cb_data->ev, 0);
	else
		event_del(cb_data->ev);
	return;
}

dbus_bool_t _bridge_add_watch(DBusWatch *watch, void *data)
{
	bridge_t *self = (bridge_t*)data;
	struct bridge_watch_data *cb_data = (struct bridge_watch_data *)malloc(sizeof(struct bridge_watch_data));
	int f = 0;

	if (dbus_watch_get_flags(watch) & DBUS_WATCH_READABLE)
		f |= EV_READ;
	if (dbus_watch_get_flags(watch) & DBUS_WATCH_WRITABLE)
		f |= EV_WRITE;

	cb_data->self = self;
	cb_data->watch = watch;
	cb_data->ev = event_new(self->event_base, dbus_watch_get_unix_fd(watch),
							f|EV_PERSIST, _bridge_handle_dbus_watch, cb_data);

	dbus_watch_set_data(watch, cb_data, 0);
	_bridge_toggle_watch(watch, cb_data);
	return TRUE;
}

void _bridge_remove_watch(DBusWatch *watch, void *data)
{
	struct bridge_watch_data *cb_data = (struct bridge_watch_data *)dbus_watch_get_data(watch);
	(void)data;

	event_free(cb_data->ev);
	free(cb_data);

	return;
}

void _bridge_toggle_timeout(DBusTimeout *timeout, void *data)
{
	struct bridge_timeout_data *cb_data = (struct bridge_timeout_data *)dbus_timeout_get_data(timeout);
	int interval = dbus_timeout_get_interval(timeout);
	struct timeval t = {
		.tv_sec = interval/1000,
		.tv_usec = interval%1000
	};
	(void)data;

	if (dbus_timeout_get_enabled(timeout))
		event_add(cb_data->ev, &t);
	else
		event_del(cb_data->ev);
	return;
}

dbus_bool_t _bridge_add_timeout(DBusTimeout *timeout, void *data)
{
	bridge_t *self = (bridge_t*)data;
	struct bridge_timeout_data *cb_data = (struct bridge_timeout_data *)malloc(sizeof(struct bridge_timeout_data));

	cb_data->self = self;
	cb_data->timeout = timeout;
	cb_data->ev = event_new(self->event_base, -1,
		EV_PERSIST, _bridge_handle_dbus_timeout, cb_data);
	dbus_timeout_set_data(timeout, cb_data, 0);
	_bridge_toggle_timeout(timeout, cb_data);
	return TRUE;
}

void _bridge_remove_timeout(DBusTimeout *timeout, void *data)
{
	struct bridge_timeout_data *cb_data = (struct bridge_timeout_data *)dbus_timeout_get_data(timeout);
	(void)data;

	event_free(cb_data->ev);
	free(cb_data);
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
		ret = bridge_request_init(self->head, self, self->socket);
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

int bridge_init(bridge_t *self, const char *socket_path, const char *dbus_bus_type)
{
	DBusError dbus_error;

	self->running = 0;

	if(!strcmp(dbus_bus_type, "system"))
		self->dbus_bus_type = DBUS_BUS_SYSTEM;
	else if(!strcmp(dbus_bus_type, "session"))
		self->dbus_bus_type = DBUS_BUS_SESSION;
	else {
		fprintf(stdout, "Unknown bus '%s'\n", dbus_bus_type);
		return EINVAL;
	}

	if (FCGX_Init() != 0) {
		fprintf(stdout, "FCGX_Init failed.");
		return EINVAL;
	}

#if CONFIG_SYSTEMD
	if(sd_listen_fds(0) > 0)
		self->socket = SD_LISTEN_FDS_START + 0;
	else
#endif
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
	if ((self->dbus_connection = dbus_bus_get(self->dbus_bus_type, &dbus_error)) == 0) {
		fprintf(stdout, "couldn't connect to dbus: %s: %s", dbus_error.name, dbus_error.message);
		return EINVAL;
	}

	self->head = 0;

	self->event_base = event_base_new();

	self->ev = event_new(self->event_base, self->socket, EV_READ|EV_PERSIST, bridge_handle_cgi, self);
	event_add(self->ev, 0);

	if (!dbus_connection_set_watch_functions(self->dbus_connection,
			_bridge_add_watch, _bridge_remove_watch,
			 _bridge_toggle_watch, self, 0))
		return EINVAL;

	if (!dbus_connection_set_timeout_functions(self->dbus_connection,
			_bridge_add_timeout, _bridge_remove_timeout,
			_bridge_toggle_timeout, self, 0))
		return EINVAL;

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
		free(request);
	}
	event_free(self->ev);
	event_base_free(self->event_base);
	return 0;
}

static void sig_handler(int fd, short event, void *arg)
{
	bridge_t *self = (bridge_t*)arg;
	(void)fd;
	(void)event;

	self->running = 0;
	event_base_loopbreak(self->event_base);
}

int bridge_run(bridge_t *self)
{
	struct event *ev_sigint;
	struct event *ev_sigquit;
	struct event *ev_sigterm;

	ev_sigquit = event_new(self->event_base, SIGQUIT,
		EV_SIGNAL|EV_PERSIST, sig_handler,  self);
	event_add(ev_sigquit, 0);
	ev_sigint = event_new(self->event_base, SIGINT,
		EV_SIGNAL|EV_PERSIST, sig_handler,  self);
	event_add(ev_sigint, 0);
	ev_sigterm = event_new(self->event_base, SIGTERM,
		EV_SIGNAL|EV_PERSIST, sig_handler,  self);
	event_add(ev_sigterm, 0);

	self->running = 1;
	while (self->running) {
		event_base_loop(self->event_base, 0);
		while (dbus_connection_dispatch(self->dbus_connection) == DBUS_DISPATCH_DATA_REMAINS);
	}

	event_free(ev_sigquit);
	event_free(ev_sigint);
	event_free(ev_sigterm);

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

