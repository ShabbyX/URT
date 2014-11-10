#! /bin/bash

# first, run the program and make sure it succeeds
# this has the side effect that .libs/lt-* is created
args="req_sem_name=MREQ res_sem_name=MRES mem_name=MMEM"
$top_srcdir/tests/execute_test.sh ./test_master $args :: ./test_slave 20 $args || exit $?

if test x"$have_valgrind" = xy; then
  # then run valgrind on the executable itself
  args="req_sem_name=VMREQ res_sem_name=VMRES mem_name=VMMEM"
  $top_srcdir/tests/execute_test.sh valgrind ./.libs/lt-test_master $args :: ./.libs/lt-test_slave 20 $args
fi
