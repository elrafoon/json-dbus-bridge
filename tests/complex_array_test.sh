#!/bin/bash

. `dirname $0`/test_setup.sh "com.pengutronix.jdb.Test.Complex" || exit

jdb_test_echo EchoArray ai "[ 1, 2, 3, 42, 99, -42 ]" &&
jdb_test_echo EchoArray ai "[ 1, 2, 3, 42, 99, \"foo\" ]" null "integer value expected." &&

jdb_test_echo CountArray ai "[ 1, 2, 3, 42, 99, -42 ]" 6 &&
jdb_test_echo CountArray ai "[]" 0 &&

args="42" &&
for i in `seq 1 10`; do
	args="$args, $i, 1$i, 10$i, 100$i, 1000$i, 2$i, 20$i, 200$i, 2000$i, 3$i, 30$i, 300$i, 3000$i, 4$i, 40$i, 400$i, 4000$i, 5$i, 50$i, 500$i, 5000$i, $args"
done &&
jdb_test_echo EchoArray ai "[ $args ]"

