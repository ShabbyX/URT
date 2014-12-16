#! /bin/bash

args="write_cond_name=KXWCND read_cond_name=KXRCND mem_name=KXMEM mutex_name=KXMTX"

$top_srcdir/tests/execute_test.sh master.ko $args :: slave.ko 10 $args
