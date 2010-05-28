#!/bin/bash

if [ "`basename $0`" = "basic_test.sh" ]; then
	. `dirname $0`/libtest.sh
fi

JDB_TEST_SERVICE="com.pengutronix.jdb.test|/"
JDB_TEST_IFACE="com.pengutronix.jdb.Test.Basic"

jdb_test_bridge || return

jdb_test_echo EchoBYTE y 30
jdb_test_echo EchoBYTE y 127
# overflow to 1
jdb_test_echo EchoBYTE y 257 1

jdb_test_echo EchoBOOLEAN b true
jdb_test_echo EchoBOOLEAN b True true
jdb_test_echo EchoBOOLEAN b false
jdb_test_echo EchoBOOLEAN b FALSE false
jdb_test_echo EchoBOOLEAN b '"foo"' null "boolean value expected."
jdb_test_echo EchoBOOLEAN b 42 null "boolean value expected."
jdb_test_echo EchoBOOLEAN b 0 null "boolean value expected."

jdb_test_echo EchoINT16 n 31244
jdb_test_echo EchoINT16 n -31244
# overflow
jdb_test_echo EchoINT16 n 32768 -32768
jdb_test_echo EchoINT16 n '"foo"' null "integer value expected."

jdb_test_echo EchoUINT16 q 40000
# overflow
jdb_test_echo EchoUINT16 q 65537 1
jdb_test_echo EchoUINT16 q -1 65535
jdb_test_echo EchoUINT16 q '"foo"' null "integer value expected."

jdb_test_echo EchoINT32 i 0
jdb_test_echo EchoINT32 i -1
jdb_test_echo EchoINT32 i 42
jdb_test_echo EchoINT32 i 2147483647
jdb_test_echo EchoINT32 i -2147483648
jdb_test_echo EchoINT32 i '"foo"' null "integer value expected."

jdb_test_echo EchoUINT32 u 42
echo "FIXME: support number > 2^31-1"
# cjson only suports "int". it returns 2^31-1 for any larger values
jdb_test_echo EchoUINT32 u 4294967000 2147483647
jdb_test_echo EchoUINT32 u '"foo"' null "integer value expected."

jdb_test_echo EchoINT64 x 0
jdb_test_echo EchoINT64 x -1
jdb_test_echo EchoINT64 x 42
jdb_test_echo EchoINT64 x '"foo"' null "integer value expected."

jdb_test_echo EchoUINT64 t 0
jdb_test_echo EchoUINT64 t 42
jdb_test_echo EchoUINT64 t '"foo"' null "integer value expected."
jdb_test_echo EchoUINT64 t true null "integer value expected."

jdb_test_echo EchoDOUBLE d 0.000000
jdb_test_echo EchoDOUBLE d 3.14159265 3.141593
jdb_test_echo EchoDOUBLE d -3.5E-2 -0.035000
jdb_test_echo EchoDOUBLE d '"foo"' null "double value expected."

jdb_test_echo EchoSTRING s '"Hello World!"'
jdb_test_echo EchoSTRING s '"öäüßµ"'
jdb_test_echo EchoSTRING s '"最高优先指数"'

jdb_test_echo EchoINT32 i "23, 42" null "Unexpected extra parameter found."
jdb_test_echo SumINT32 ii "23" null "Aditional parameter expexted."

