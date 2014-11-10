#! /bin/bash

# first, run the program and make sure it succeeds
# this has the side effect that .libs/lt-* is created
$top_srcdir/tests/execute_test.sh ./test_master || exit $?

if test x"$have_valgrind" = xy; then
  # then run valgrind on the executable itself
  $top_srcdir/tests/execute_test.sh valgrind ./.libs/lt-test_master
fi
