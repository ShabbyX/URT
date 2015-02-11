#! /bin/bash

# first, run the program and make sure it succeeds
# this has the side effect that .libs/lt-* is created
args="sem_name=XSEM"
$top_srcdir/tests/execute_test.sh ./test_master $args test_arg=123456 test_arg2 test-arg3='"abc def,ghi"' \
  :: ./test_slave 20 $args test_arg='"x y"' test_arg2=1,2,3 || exit $?

if test x"$have_valgrind" = xy; then
  # then run valgrind on the executable itself
  args="sem_name=VXSEM"
  $top_srcdir/tests/execute_test.sh ./.libs/lt-test_master $args test_arg=123456 test_arg2 test-arg3='"abc def,ghi"' \
    :: ./.libs/lt-test_slave 20 $args test_arg='"x y"' test_arg2=1,2,3
fi
