ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

TARGET := test.ko

include ../../Makefile.test.k

.PHONY: all
all: main.c modules
	@$(SHELL_NOOP)

endif
