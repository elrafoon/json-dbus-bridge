#!/bin/bash
#
# json dbus bridge
#
# Copyright (c) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# To the extent possible under law, Pengutronix e.K. has waived all copyright
# and related or neighboring rights to JSON-DBus-Bridge Demo Code.
#

. `dirname $0`/libtest.sh

export JDB_TEST_IFACE="$1"

jdb_test_bridge 2> /dev/null || return

