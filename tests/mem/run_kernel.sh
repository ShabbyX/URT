#! /bin/bash

args="req_sem_name=KMREQ res_sem_name=KMRES mem_name=KMMEM"

$top_srcdir/tests/execute_test.sh master.ko $args :: slave.ko 20 $args
