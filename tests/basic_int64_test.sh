#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || return

jdb_test_echo EchoINT64 x 0 &&
jdb_test_echo EchoINT64 x -1 &&
jdb_test_echo EchoINT64 x 42 &&
jdb_test_echo EchoINT64 x '"foo"' null "integer value expected."

