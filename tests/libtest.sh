#!/bin/bash
#
#  json dbus bridge
#
#  Copyright (c) 2009 by Michael Olbrich <m.olbrich@pengutronix.de>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation; either version 2.1 of
#  the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

if [ -z "$libtest_tag" ]; then
	libtest_tag=1
	id=0
	error_count=0
fi

jdb_status() {
	echo
	echo $error_count tests failed.
	return $error_count
}

jdb_call() {
	local url="$JDB_TEST_URL"
	local data="$1"

	local result=`tempfile`
	local debug=`tempfile`

	if [ -z "$url" ]; then
		echo "FATAL: URL not set" 1>&2
		exit 1
	fi

	wget -e no_proxy=localhost -O "$result" --post-data="$data" "$url" > "$debug" 2>&1
	status=$?
	if [ $status -ne 0 ]; then
		echo 1>&2
		cat "$debug" 1>&2
	else
		cat "$result"
	fi
	rm "$result" "$debug"
	return $status
}

jdb_test_bridge() {
	local data='{"service":"org.freedesktop.DBus|/","method":"org.freedesktop.DBus.Introspectable.Introspect","id":0,"params": []}'

	jdb_call "$data" > /dev/null
}

jdb_test() {
	local service="$JDB_TEST_SERVICE"
	local iface="$JDB_TEST_IFACE"
	local method="${iface:+${iface}.}$1"
	local params="$2"
	local response="${3:-null}"
	local error
	if [ -z "$4" ]; then
		error=null
	else
		error="{ \"origin\": 1, \"code\": 1, \"message\": \"$4\" }"
	fi
	id=$[id+1]

	local data="{ \"service\":\"$service\", \"method\":\"$method\", \"id\":$id, \"params\":$params }"
	local expect="{ \"id\": $id, \"error\": $error, \"result\": $response }"

	if [ "$JDB_TEST_VERBOSE" = "yes" ]; then
		echo -n "$method($params)"
	fi
	local result
	result=`jdb_call "$data"`
	if [ $? -ne 0 ]; then
		echo " Failed:"
		echo Sent:     $data
		error_count=$[error_count+1]
		return 1
	fi
	if [ "x$result" != "x$expect" ]; then
		echo " Failed:"
		echo Sent:     $data
		echo Expected: $expect
		echo Got:      $result
		echo
		error_count=$[error_count+1]
		return 1
	else
		if [ "$JDB_TEST_VERBOSE" = "yes" ]; then
			echo "."
		fi
	fi
}

jdb_test_echo() {
	local method="$1"
	local type="$2"
	local value="$3"
	local response="${4:-$value}"
	local error="$5"

	jdb_test "$method" "[ \"$type\", $value ]" "$response" "$error"
}


