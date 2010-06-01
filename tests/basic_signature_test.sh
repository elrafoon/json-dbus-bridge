#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || exit

jdb_test_echo EchoINT32 i "23, 42" null "Unexpected extra parameter found."
jdb_test_echo SumINT32 ii "23" null "Aditional parameter expexted."

