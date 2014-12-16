#! /bin/bash

# first, run the program and make sure it succeeds
# this has the side effect that .libs/lt-* is created
args="write_cond_name=XWCND read_cond_name=XRCND mem_name=XMEM mutex_name=XMTX"
$top_srcdir/tests/execute_test.sh ./test_master $args :: ./test_slave 10 $args || exit $?

if test x"$have_valgrind" = xy; then
  # then run valgrind on the executable itself
  args="write_cond_name=VXWCND read_cond_name=VXRCND mem_name=VXMEM mutex_name=VXMTX"
  $top_srcdir/tests/execute_test.sh valgrind ./.libs/lt-test_master $args :: ./.libs/lt-test_slave 10 $args
fi
