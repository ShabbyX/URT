#! /bin/bash

# first, run the program and make sure it succeeds
# this has the side effect that .libs/lt-* is created
args="rwlock_name=XRWL"
$top_srcdir/tests/execute_test.sh ./test_master $args :: ./test_slave 4 $args || exit $?

if test x"$have_valgrind" = xy; then
  # then run valgrind on the executable itself
  args="rwlock_name=VXRWL"
  $top_srcdir/tests/execute_test.sh valgrind ./.libs/lt-test_master $args :: ./.libs/lt-test_slave 4 $args
fi
