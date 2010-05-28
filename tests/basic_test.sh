#!/bin/bash

if [ "`basename $0`" = "basic_test.sh" ]; then
	. `dirname $0`/libtest.sh
fi

JDB_TEST_SERVICE="com.pengutronix.jdb.test|/"
JDB_TEST_IFACE="com.pengutronix.jdb.Test.Basic"

jdb_test_bridge || return

jdb_test_echo EchoBYTE y 30
jdb_test_echo EchoBYTE y 127
jdb_test_echo EchoBYTE y 1000

jdb_test_echo EchoBOOLEAN b true
jdb_test_echo EchoBOOLEAN b True true
jdb_test_echo EchoBOOLEAN b false
jdb_test_echo EchoBOOLEAN b FALSE false
jdb_test_echo EchoBOOLEAN b '"foo"' null "boolean value expected."
jdb_test_echo EchoBOOLEAN b 42 null "boolean value expected."
jdb_test_echo EchoBOOLEAN b 0 null "boolean value expected."

