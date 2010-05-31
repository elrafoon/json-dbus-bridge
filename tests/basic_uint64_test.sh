#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || return

jdb_test_echo EchoUINT64 t 0 &&
jdb_test_echo EchoUINT64 t 42 &&
jdb_test_echo EchoUINT64 t '"foo"' null "integer value expected." &&
jdb_test_echo EchoUINT64 t true null "integer value expected."

