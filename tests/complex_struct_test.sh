#!/bin/bash
#
# json dbus bridge
#
# Copyright (c) 2010 by Rolf Offermanns <roffermanns@sysgo.com>
#
# To the extent possible under law, SYSGO AG has waived all copyright
# and related or neighboring rights to JSON-DBus-Bridge Demo Code.
#

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Complex" || exit

jdb_test_echo EchoStruct "(aiuaiu)" "[ [ 1, 2, 3, 42, 99, -42 ], 4711, [ -42, 99, 42, 3, 2, 1 ], 1701 ]" &&
jdb_test_echo EchoStructArray "a(aiu)" "[ [ [ 1, 2, 3, 42, 99, -42 ], 4711 ], [ [ -42, 99, 42, 3, 2, 1 ], 1701 ] ]" &&
jdb_test_echo CountStructIntegerValues "a(aiu)" "[ [ [ 1, 2, 3, 42, 99, -42 ], 4711 ], [ [ -42, 99, 42, 3, 2, 1 ], 1701 ] ]" 6622 &&
jdb_test_echo EchoStruct "(aiuaiu)" "[ [ 1, 2, null, 42, 99, -42 ], 4711, [ -42, 99, 42, 3, 2, 1 ], 1701 ]" null "unexpected 'null' element in json array." &&
jdb_test_echo EchoStruct "(aiuaiu)" "[ [ 1, 2, 3, 42, 99, -42 ], 4711, [ -42, 99, 42, 3, 2, 1 ] ]" null "Aditional parameter expexted." &&
jdb_test_echo EchoStruct "(aiuaiu)" "[ [ 1, 2, 3, 42, 99, -42 ], 4711, [ -42, 99, 42, 3, 2, 1 ], 1701, 12 ]" null "Unexpected extra parameter found."
