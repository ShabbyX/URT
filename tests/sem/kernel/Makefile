ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

TARGET := test.ko
TARGET2 := wait.ko
COUNT := 20

include ../../Makefile.test.k

.PHONY: all
all: main.c wait.c modules
	@$(SHELL_NOOP)

%.c: ../%.c
	@$(CP) '$<' '$@'

endif
