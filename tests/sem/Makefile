TARGET := test
TARGET_ARGS := test_arg=123456 test_arg2 test_arg3="abc def",ghi
TARGET2 := wait
TARGET2_ARGS := test_arg="x y" test_arg2=1,2,3
COUNT := 20
USE_SHARED := y

include ../Makefile.test.u

Makefile.dep: main.c wait.c

$(TARGET): main.o
$(TARGET2): wait.o
