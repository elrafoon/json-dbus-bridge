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

#ifndef _SYSTEMINFO_SKEL_H_
#define _SYSTEMINFO_SKEL_H_

#include <dbus/dbus.h>

typedef struct system_info system_info_t;

struct system_info {
	int (*Date)(system_info_t *self, unsigned int *date);
	int (*Noop)(system_info_t *self);
	int (*Ping)(system_info_t *self, const char *ping, const char **pong);
	DBusConnection *connection;
	char *path;
};

int system_info_init(system_info_t *self);
int system_info_destroy(system_info_t *self);
int system_info_register(system_info_t *self, DBusConnection *c, const char *path);

#endif /* _SYSTEMINFO_SKEL_H_ */

