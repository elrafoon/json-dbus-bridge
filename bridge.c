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

#include <dbus/dbus.h>

#include "bridge.h"

int bridge_init(bridge_t *self, const char *socket_path)
{
	int ret;
	int socket = 0;
	DBusError dbus_error;
	DBusConnection *dbus_connection;

	if (FCGX_Init() != 0) {
		fprintf(stdout, "FCGX_Init failed.");
		return EINVAL;
	}

	if (socket_path) {
		if ((socket = FCGX_OpenSocket(socket_path, SOCKET_BACKLOG)) < 0) {
			fprintf(stdout, "FCGX_OpenSocket failed: \"%s\".",
					   socket_path);
			return EINVAL;
		}
	}


	/* attach us to dbus */
	dbus_error_init(&dbus_error);
	if ((dbus_connection = dbus_bus_get(DBUS_BUS_SESSION, &dbus_error)) == 0) {
		fprintf(stdout, "couldn't connect to dbus: %s: %s", dbus_error.name, dbus_error.message);
		return EINVAL;
	}

	if ((ret = bridge_request_init(&self->request, dbus_connection, socket)) != 0)
		return ret;

	return 0;
}

int bridge_destroy(bridge_t *self)
{
	return bridge_request_destroy(&self->request);
}

int bridge_run(bridge_t *self)
{
	int ret;

	while (bridge_request_accept(&self->request) == 0) {
		if ((ret = bridge_request_handle(&self->request)) != 0)
			return ret;
	}
	return 0;
}

