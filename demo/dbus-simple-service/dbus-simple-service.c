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

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>

#include <systeminfo_skel.h>


int system_info_date(system_info_t *self, unsigned int *date)
{
	(void)self;
	struct timeval t;
	gettimeofday(&t, 0);
	*date = t.tv_sec;
	return 0;
}

int system_info_noop(system_info_t *self)
{
	(void)self;
	return 0;
}

int system_info_ping(system_info_t *self, const char *ping, const char **pong)
{
	(void)self;
	*pong = ping;
	return 0;
}

static int quit;

void sighandler(int signal)
{
	(void)signal;
	quit = 1;
}

int main(void)
{
	int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGTERM, 0 };
	struct sigaction action = {
		.sa_handler = sighandler
	};
	int *signal;
	DBusConnection *connection;
	DBusError err;
	system_info_t info;

	quit = 0;
	sigemptyset(&action.sa_mask);
	for (signal = signals; *signal; ++signal) {
		sigaddset(&action.sa_mask, *signal);
	}
	for (signal = signals; *signal; ++signal) {
		if (sigaction(*signal, &action, NULL) != 0)
			return 1;
	}

	dbus_error_init(&err);
	if ((connection = dbus_bus_get(DBUS_BUS_SESSION, &err)) == 0) {
		fprintf(stderr, "couldn't connect to dbus: %s: %s\n",
			err.name, err.message);
		dbus_error_free(&err);
		exit(1);
	}
	if (dbus_bus_request_name(connection, "com.pengutronix.jdb.simpleinfo",
			 0, &err) < 0) {
		fprintf(stderr, "couldn't connect to dbus: %s: %s\n",
			err.name, err.message);
		dbus_error_free(&err);
		exit(1);
	}
	dbus_error_free(&err);

	system_info_init(&info);
	info.Date = system_info_date;
	info.Noop = system_info_noop;
	info.Ping = system_info_ping;
	if (system_info_register(&info, connection, "/") != 0) {
		fprintf(stderr, "system_info_register failed.\n");
		exit(1);
	}

	while (!quit) {
		dbus_connection_read_write_dispatch(connection, 1000);
	}
	dbus_connection_unref(connection);
	return 0;
}

