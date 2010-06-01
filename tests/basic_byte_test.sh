#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || exit

jdb_test_echo EchoBYTE y 30 &&
jdb_test_echo EchoBYTE y 127 &&
# overflow to 1
jdb_test_echo EchoBYTE y 257 1

