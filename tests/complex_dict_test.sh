#!/bin/bash
#
# json dbus bridge
#
# Copyright (c) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# To the extent possible under law, Pengutronix e.K. has waived all copyright
# and related or neighboring rights to JSON-DBus-Bridge Demo Code.
#

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Complex" || exit

jdb_test_echo EchoDict 'a{si}' '{ "a": 1, "b": 2, "ff": 3, "y": 42, "x": 99, "foo": -42 }' &&
jdb_test_echo EchoDict 'a{si}' '{ "a": 1, "b": "2" }' null "integer value expected." &&
jdb_test_echo EchoDict 'a{si}' '{ "a": 1, "b": null }' null "unexpected 'null' value in json object." &&
jdb_test_echo DictKeys 'a{si}' '{ "a": 1, "b": 2, "c": 55 }' '[ "a", "b", "c" ]'

