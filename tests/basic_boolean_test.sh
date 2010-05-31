#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Basic" || return

jdb_test_echo EchoBOOLEAN b true &&
jdb_test_echo EchoBOOLEAN b True true &&
jdb_test_echo EchoBOOLEAN b false &&
jdb_test_echo EchoBOOLEAN b FALSE false &&
jdb_test_echo EchoBOOLEAN b '"foo"' null "boolean value expected." &&
jdb_test_echo EchoBOOLEAN b 42 null "boolean value expected." &&
jdb_test_echo EchoBOOLEAN b 0 null "boolean value expected."

