#! /bin/bash

args="rwlock_name=KXRWL"

$top_srcdir/tests/execute_test.sh master.ko $args :: slave.ko 4 $args
