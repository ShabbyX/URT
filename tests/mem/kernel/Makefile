ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

TARGET := test.ko
TARGET2 := add.ko
COUNT := 20

include ../../Makefile.test.k

.PHONY: all
all: main.c add.c modules
	@$(SHELL_NOOP)

endif
