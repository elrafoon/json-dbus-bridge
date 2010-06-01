#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || exit

jdb_test_echo EchoINT16 n 31244 &&
jdb_test_echo EchoINT16 n -31244 &&
# overflow
jdb_test_echo EchoINT16 n 32768 -32768 &&
jdb_test_echo EchoINT16 n '"foo"' null "integer value expected."

