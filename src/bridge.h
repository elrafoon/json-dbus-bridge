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

#ifndef BRIDGE_H
#define BRIDGE_H

#define SOCKET_BACKLOG 10

#include <event.h>
#include "bridge_request.h"

struct bridge {
	bridge_request_t *head;
	DBusBusType dbus_bus_type;
	DBusConnection *dbus_connection;
	int socket;
	struct event_base *event_base;
	struct event *ev;
	int running;
};

struct bridge_timeout_data {
	struct bridge *self;
	DBusTimeout *timeout;
	struct event *ev;
};

struct bridge_watch_data {
	struct bridge *self;
	DBusWatch *watch;
	struct event *ev;
};

int bridge_init(bridge_t *self, const char *socket_path, const char *dbus_bus_type);
int bridge_destroy(bridge_t *self);

int bridge_run(bridge_t *self);

#endif /* BRIDGE_H */

