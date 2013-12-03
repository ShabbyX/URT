ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

TARGET := test.ko
TARGET_ARGS := test_arg=123456 test_arg2 test-arg3='"abc def,ghi"'
TARGET2 := wait.ko
TARGET2_ARGS := test_arg='"x y"' test_arg2=1,2,3
COUNT := 20

include ../../Makefile.test.k

.PHONY: all
all: main.c wait.c modules
	@$(SHELL_NOOP)

endif
