#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || exit

jdb_test_echo EchoDOUBLE d 0.000000 &&
jdb_test_echo EchoDOUBLE d 3.14159265 3.141593 &&
jdb_test_echo EchoDOUBLE d -3.5E-2 -0.035000 &&
jdb_test_echo EchoDOUBLE d '"foo"' null "double value expected."

