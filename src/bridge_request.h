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

#ifndef BRIDGE_REQUEST_H
#define BRIDGE_REQUEST_H

#include <dbus/dbus.h>

#include "fcgiapp.h"

typedef struct bridge_request bridge_request_t;
typedef struct bridge bridge_t;

typedef enum {
	error_origin_server = 1,
	error_origin_method = 2,
	error_origin_transport = 3,
	error_origin_client = 4
} error_origin_t;

typedef enum {
	error_code_illegal_service = 1,
	error_code_service_not_found = 2,
	error_code_class_not_found = 3,
	error_code_method_not_found = 4,
	error_code_parameter_missmatch = 5,
	error_code_permission_denied = 6
} error_code_t;

struct bridge_request {
	FCGX_Request request;
	int id;
	struct json_tokener *tokener;
	bridge_t *bridge;
	bridge_request_t *next;
	struct json_object *response;
};

int bridge_request_init(bridge_request_t *self, bridge_t *bridge, int socket);
int bridge_request_destroy(bridge_request_t *self);

int bridge_request_accept(bridge_request_t *self);
int bridge_request_handle(bridge_request_t *self);

#endif /* BRIDGE_REQUEST_H */

