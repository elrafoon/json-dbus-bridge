#!/bin/bash

. `dirname $0`/libtest.sh

export JDB_TEST_IFACE="$1"

jdb_test_bridge || return

