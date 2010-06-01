#!/bin/bash
#
# json dbus bridge
#
# Copyright (c) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# To the extent possible under law, Pengutronix e.K. has waived all copyright
# and related or neighboring rights to JSON-DBus-Bridge Demo Code.
#

if [ -z "$libtest_tag" ]; then
	libtest_tag=1
	id=0
fi

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
		if [ ${#params} -gt 50 ]; then
			echo -n "$method(..[${#params}]..)"
		else
			echo -n "$method($params)"
		fi
	fi
	local result
	result=`jdb_call "$data"`
	if [ $? -ne 0 ]; then
		echo " Failed:"
		echo Sent:     $data
		return 1
	fi
	if [ "x$result" != "x$expect" ]; then
		echo " Failed:"
		echo "Sent:     $data"
		echo "Expected: $expect"
		echo "Got:      $result"
		echo
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


