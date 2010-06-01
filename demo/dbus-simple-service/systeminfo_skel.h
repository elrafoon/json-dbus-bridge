/*
 * json dbus bridge
 *
 * Copyright (c) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
 *
 * To the extent possible under law, Pengutronix e.K. has waived all copyright
 * and related or neighboring rights to JSON-DBus-Bridge Demo Code.
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

