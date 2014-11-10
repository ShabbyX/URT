#! /bin/bash

module_path=$srcdir

args="req_sem_name=KMREQ res_sem_name=KMRES mem_name=KMMEM"

$top_srcdir/tests/execute_test.sh "$module_path"/master.ko $args :: "$module_path"/slave.ko 20 $args
