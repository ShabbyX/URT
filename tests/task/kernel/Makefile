ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

include ../../Makefile.test.k

.PHONY: all
all: main.c modules
	@$(SHELL_NOOP)

%.c: ../%.c
	@$(CP) '$<' '$@'

endif
