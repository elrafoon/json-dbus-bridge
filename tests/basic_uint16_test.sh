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

jdb_test_echo EchoUINT16 q 40000 &&
# overflow
jdb_test_echo EchoUINT16 q 65537 1 &&
jdb_test_echo EchoUINT16 q -1 65535 &&
jdb_test_echo EchoUINT16 q '"foo"' null "integer value expected."

