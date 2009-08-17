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

#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <json_tokener.h>
#include <dbus/dbus.h>

#include "bridge_request.h"

int bridge_request_init(bridge_request_t *self, DBusConnection *dbus_connection, int socket)
{

	if (FCGX_InitRequest(&self->request, socket, FCGI_FAIL_ACCEPT_ON_INTR) != 0) {
		return EINVAL;
	}
	self->tokener = json_tokener_new();
	self->dbus_connection = dbus_connection;
	return 0;
}

int bridge_request_destroy(bridge_request_t *self)
{
	json_tokener_free(self->tokener);
	FCGX_Free(&self->request, TRUE);
	return 0;
}

int bridge_request_accept(bridge_request_t *self)
{
	return FCGX_Accept_r(&self->request);
}

int bridge_request_getinput(bridge_request_t *self, char **data)
{
	char *contentLength;
	char *buffer;
	int len = 0;

	if ((contentLength = FCGX_GetParam("CONTENT_LENGTH", self->request.envp)) != 0)
		len = strtol(contentLength, NULL, 10);

	if (len <= 0)
		return EINVAL;

	if ((buffer = malloc(len)) == 0) {
		FCGX_FPrintF(self->request.err, "out of memory!");
		return ENOMEM;
	}
	if (FCGX_GetStr(buffer, len, self->request.in) != len) {
		FCGX_FPrintF(self->request.err, "Got less data than expected.");
		return EINVAL;
	}
	*data = buffer;
	return 0;
}

void bridge_request_transmit(bridge_request_t *self, struct json_object *obj)
{
	FCGX_FPrintF(self->request.out, "Content-type: application/json\r\n\r\n");
	FCGX_FPrintF(self->request.out, json_object_to_json_string(obj));
}

void bridge_request_error(bridge_request_t *self)
{
	FCGX_FPrintF(self->request.out, "Status: 400 Bad Request\r\n\r\n");
}

int bridge_request_create_response(bridge_request_t *self, int id, struct json_object **response,
				   struct json_object *error, struct json_object *result)
{
	struct json_object *obj;

	if (!self)
		return EINVAL;

	if ((obj = json_object_new_object()) == 0)
		return ENOMEM;

	json_object_object_add(obj, "id", json_object_new_int(id));
	json_object_object_add(obj, "error", error);
	json_object_object_add(obj, "result", result);

	*response = obj;
	return 0;
}

void bridge_request_json_error(bridge_request_t *self, int id, error_origin_t origin,
			       error_code_t code, const char *message)
{
	struct json_object *response, *error;
	if ((error = json_object_new_object()) == 0)
		goto failed;

	json_object_object_add(error, "origin", json_object_new_int(origin));
	json_object_object_add(error, "code", json_object_new_int(code));
	json_object_object_add(error, "message", json_object_new_string((char *)message));

	if (bridge_request_create_response(self, id, &response, error, 0) != 0)
		goto failed;

	bridge_request_transmit(self, response);

	json_object_put(response);
	return;
failed:
	bridge_request_error(self);
}

int _json_object_object_getint(struct json_object *obj, char *key, int *value)
{
	struct json_object *tmp;

	if (((tmp = json_object_object_get(obj, key)) == 0) ||
	    !json_object_is_type(tmp, json_type_int))
		return EINVAL;
	*value = json_object_get_int(tmp);
	return 0;
}

int _json_object_object_getstring(struct json_object *obj, char *key, const char **value)
{
	struct json_object *tmp;

	if (((tmp = json_object_object_get(obj, key)) == 0) ||
	    !json_object_is_type(tmp, json_type_string))
		return EINVAL;
	*value = json_object_get_string(tmp);
	return 0;
}

int bridge_request_dbus_params(bridge_request_t *self, int id, struct json_object *params,
			       DBusMessageIter *it)
{
	int len, i;
	struct json_object *tmp;
	int type = 0;
	int int_value;
	int64_t ll_value;
	double double_value;
	const char *str_value;
	void *value = 0;
	const char *data;

	len = json_object_array_length(params);

	for (i = 0; i < len; ++i) {
		tmp = json_object_array_get_idx(params, i);
		if (json_object_get_type(tmp) != json_type_string) {
			bridge_request_json_error(self, id, error_origin_server,
						  error_code_illegal_service,
						  "unsupported json argument type.");
			return EINVAL;
		}
		data = json_object_get_string(tmp);
		type = data[0];
		switch (data[0]) {
			case DBUS_TYPE_BOOLEAN:
				int_value = (strcasecmp(data+1, "true") == 0);
				value = &int_value;
				break;
			case DBUS_TYPE_DOUBLE:
				double_value = atof(data+1);
				value = &double_value;
				break;
			case DBUS_TYPE_BYTE:
			case DBUS_TYPE_INT16:
			case DBUS_TYPE_UINT16:
			case DBUS_TYPE_INT32:
			case DBUS_TYPE_UINT32:
				int_value = atoi(data+1);
				value = &int_value;
				break;
			case DBUS_TYPE_INT64:
			case DBUS_TYPE_UINT64:
				ll_value = atoll(data+1);
				value = &ll_value;
			case DBUS_TYPE_STRING:
			case DBUS_TYPE_OBJECT_PATH:
			case DBUS_TYPE_SIGNATURE:
				str_value = data+1;
				value = &str_value;
				break;
			default:
				bridge_request_json_error(self, id, error_origin_server,
							  error_code_illegal_service,
							  "unsupported json argument type.");
				return EINVAL;
		}
		if (!dbus_message_iter_append_basic(it, type, value)) {
			bridge_request_json_error(self, id, error_origin_server,
						  error_code_illegal_service,
						  "out of memory.");
			return ENOMEM;
		}
	}
	return 0;
}

int bridge_request_json_params(bridge_request_t *self, int id, DBusMessageIter *it,
			       struct json_object **result)
{
	struct json_object *tmp;
	struct json_object *array = 0;

	if (dbus_message_iter_has_next(it))
		array = json_object_new_array();

	do {
		switch (dbus_message_iter_get_arg_type(it)) {
			case DBUS_TYPE_STRING:
			case DBUS_TYPE_SIGNATURE:
			case DBUS_TYPE_OBJECT_PATH: {
				char *value;
				dbus_message_iter_get_basic(it, &value);
				tmp = json_object_new_string(value);
				break;
			}
			case DBUS_TYPE_INT16:
			case DBUS_TYPE_UINT16:
			case DBUS_TYPE_INT32:
			case DBUS_TYPE_BYTE: {
				int value;
				dbus_message_iter_get_basic(it, &value);
				tmp = json_object_new_int(value);
				break;
			}
			case DBUS_TYPE_DOUBLE: {
				double value;
				dbus_message_iter_get_basic(it, &value);
				tmp = json_object_new_double(value);
				break;
			}
			case DBUS_TYPE_BOOLEAN: {
				int value;
				dbus_message_iter_get_basic(it, &value);
				tmp = json_object_new_boolean(value);
				break;
			}
			default:
				tmp = 0;
				break;
		}
		if (!tmp) {
			bridge_request_json_error(self, id, error_origin_server,
						  error_code_illegal_service,
						  "unsupported dbus argument type.");
				return EINVAL;
		}
		if (array)
			json_object_array_add(array, tmp);
	} while (dbus_message_iter_next(it));

	if (array)
		*result = array;
	else
		*result = tmp;

	return 0;
}

int bridge_request_call_json_dbus(bridge_request_t *self, struct json_object *in_json,
				  struct json_object **out_json)
{
	struct json_object *params, *result;
	const char *service, *iface;
	char *method, *path;
	int id;
	int ret;
	DBusMessage *msg, *reply;
	DBusMessageIter it;
	DBusError error;


	if (!json_object_is_type(in_json, json_type_object) ||
	    (_json_object_object_getint(in_json, "id", &id) != 0)) {
		bridge_request_error(self);
		return EINVAL;
	}
	if ((ret = _json_object_object_getstring(in_json, "service", &service) != 0)) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"No service specified.");
		return ret;
	}
	if ((ret = _json_object_object_getstring(in_json, "method", &iface) != 0)) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"No method specified.");
		return ret;
	}
	if ((path = strchr(service, '|')) == 0) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"Service must be '<dbus service>|<dbus path>'.");
		return EINVAL;
	}
	*path = '\0';
	++path;
	if ((method = strrchr(iface, '.')) == 0) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"Method must be '<interface>.<method>'.");
		return EINVAL;
	}
	*method = '\0';
	++method;
	if (((params = json_object_object_get(in_json, "params")) == 0) ||
	    !json_object_is_type(params, json_type_array)) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"'param' arg missing or not an array.");
		return EINVAL;
	}

	if ((msg = dbus_message_new_method_call(service, path, iface, method)) == NULL) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"Out of memory.");
		return ENOMEM;
	}
	dbus_message_iter_init_append(msg, &it);
	if ((ret = bridge_request_dbus_params(self, id, params, &it)) != 0)
		goto send_fail;

	dbus_error_init(&error);
	if ((reply = dbus_connection_send_with_reply_and_block(self->dbus_connection, msg,
				 -1, &error)) == NULL) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			error.message);
		ret = EIO;
		goto send_fail;
	}
	if (dbus_message_iter_init(reply, &it)) {
		if ((ret = bridge_request_json_params(self, id, &it, &result)) != 0)
			goto fail_recv;
	}
	else
		result = 0;

	if ((ret = bridge_request_create_response(self, id, out_json, 0, result)) != 0) {
		bridge_request_json_error(self, id, error_origin_server, error_code_illegal_service,
			"Out of memory.");
	}

fail_recv:
	dbus_message_unref(reply);
send_fail:
	dbus_message_unref(msg);
	return ret;
}

int bridge_request_handle(bridge_request_t *self)
{
	int ret;
	char *buffer;
	struct json_object *in_json, *out_json;

	if ((ret = bridge_request_getinput(self, &buffer)) != 0) {
		bridge_request_error(self);
		return 0;
	}
	in_json = json_tokener_parse_ex(self->tokener, buffer, -1);
	if (!in_json) {
		bridge_request_error(self);
		goto out;
	}

	if ((ret = bridge_request_call_json_dbus(self, in_json, &out_json)) != 0)
		goto out;

	bridge_request_transmit(self, out_json);

	json_object_put(out_json);
out:
	json_object_put(in_json);
	json_tokener_reset(self->tokener);
	free(buffer);
	FCGX_Finish_r(&self->request);
	return 0;
}

