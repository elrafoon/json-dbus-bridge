#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || return

jdb_test_echo EchoUINT32 u 42 &&
echo "FIXME: support number > 2^31-1"
# cjson only suports "int". it returns 2^31-1 for any larger values
jdb_test_echo EchoUINT32 u 4294967000 2147483647 &&
jdb_test_echo EchoUINT32 u '"foo"' null "integer value expected."

