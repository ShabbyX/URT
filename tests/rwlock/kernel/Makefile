ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

TARGET := test.ko
TARGET2 := read.ko
COUNT := 4

include ../../Makefile.test.k

.PHONY: all
all: main.c read.c modules
	@$(SHELL_NOOP)

%.c: ../%.c
	@$(CP) '$<' '$@'

endif
