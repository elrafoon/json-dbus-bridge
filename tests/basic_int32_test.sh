#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || return

jdb_test_echo EchoINT32 i 0 &&
jdb_test_echo EchoINT32 i -1 &&
jdb_test_echo EchoINT32 i 42 &&
jdb_test_echo EchoINT32 i 2147483647 &&
jdb_test_echo EchoINT32 i -2147483648 &&
jdb_test_echo EchoINT32 i '"foo"' null "integer value expected."

