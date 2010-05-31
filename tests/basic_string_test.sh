#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || return

jdb_test_echo EchoSTRING s '"Hello World!"' &&
jdb_test_echo EchoSTRING s '"öäüßµ"' &&
jdb_test_echo EchoSTRING s '"最高优先指数"'

