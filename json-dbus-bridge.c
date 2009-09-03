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

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "bridge.h"

int main(int argc, char *argv[])
{
	int ret;
	bridge_t bridge;
	const char *socket = 0;

	if (argc == 2)
		socket = argv[1];

	if ((ret = bridge_init(&bridge, socket)) != 0) {
		fprintf(stderr, "bridge_init failed");
		goto out_return;
	}

	ret = bridge_run(&bridge);

	bridge_destroy(&bridge);
out_return:
	return ret;
}
