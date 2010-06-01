#!/bin/bash
#
# json dbus bridge
#
# Copyright (c) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# To the extent possible under law, Pengutronix e.K. has waived all copyright
# and related or neighboring rights to JSON-DBus-Bridge Demo Code.
#

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || exit

jdb_test_echo EchoBOOLEAN b true &&
jdb_test_echo EchoBOOLEAN b True true &&
jdb_test_echo EchoBOOLEAN b false &&
jdb_test_echo EchoBOOLEAN b FALSE false &&
jdb_test_echo EchoBOOLEAN b '"foo"' null "boolean value expected." &&
jdb_test_echo EchoBOOLEAN b 42 null "boolean value expected." &&
jdb_test_echo EchoBOOLEAN b 0 null "boolean value expected."

