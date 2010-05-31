#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Complex" || return

jdb_test_echo EchoVariant v '[ "i", 42 ]' 42 &&
jdb_test_echo EchoVariant v '[ "s", "foo" ]' '"foo"' &&
jdb_test_echo EchoVariant v '[ "s", "42" ]' '"42"' &&

jdb_test_echo EchoVariant v '[ "s", "foo", 33 ]' null "Unexpected extra parameter found." &&
jdb_test_echo EchoVariant v '[ "si", "foo", 33 ]' null "invalid variant signature." &&

jdb_test_echo EchoVariant v '[ "as", [ "42", "foobar"] ]' '[ "42", "foobar" ]' &&
jdb_test_echo EchoVariantArray av '[ [ "i", 42 ], ["s", "foobar"] ]' '[ 42, "foobar" ]'

