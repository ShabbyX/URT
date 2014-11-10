#! /bin/bash

module_path=$srcdir

args="sem_name=KSEM"

$top_srcdir/tests/execute_test.sh "$module_path"/master.ko $args test_arg=123456 test_arg2 test-arg3='"abc def,ghi"' \
  :: "$module_path"/slave.ko 20 $args test_arg='"x y"' test_arg2=1,2,3
