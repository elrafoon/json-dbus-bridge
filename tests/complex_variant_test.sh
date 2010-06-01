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

jdb_test_echo EchoVariant v '[ "i", 42 ]' 42 &&
jdb_test_echo EchoVariant v '[ "s", "foo" ]' '"foo"' &&
jdb_test_echo EchoVariant v '[ "s", "42" ]' '"42"' &&

jdb_test_echo EchoVariant v '[ "s", "foo", 33 ]' null "Unexpected extra parameter found." &&
jdb_test_echo EchoVariant v '[ "si", "foo", 33 ]' null "invalid variant signature." &&

jdb_test_echo EchoVariant v '[ "as", [ "42", "foobar"] ]' '[ "42", "foobar" ]' &&
jdb_test_echo EchoVariantArray av '[ [ "i", 42 ], ["s", "foobar"] ]' '[ 42, "foobar" ]'
jdb_test_echo EchoVariantArray av '[["i", 42], ["v", ["s", "foobar"]]]' '[ 42, "foobar" ]'

