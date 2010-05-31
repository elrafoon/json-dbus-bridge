#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Complex" || return

jdb_test_echo EchoDict 'a{si}' '{ "a": 1, "b": 2, "ff": 3, "y": 42, "x": 99, "foo": -42 }' &&
jdb_test_echo EchoDict 'a{si}' '{ "a": 1, "b": "2" }' null "integer value expected." &&
jdb_test_echo DictKeys 'a{si}' '{ "a": 1, "b": 2, "c": 55 }' '[ "a", "b", "c" ]'

