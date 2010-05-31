#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Complex" || return

jdb_test_echo EchoArray ai "[ 1, 2, 3, 42, 99, -42 ]" &&
jdb_test_echo EchoArray ai "[ 1, 2, 3, 42, 99, \"foo\" ]" null "integer value expected." &&

jdb_test_echo CountArray ai "[ 1, 2, 3, 42, 99, -42 ]" 6 &&
jdb_test_echo CountArray ai "[]" 0

