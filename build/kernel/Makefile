ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

include ../../Makefile.config
include ../../Makefile.common

.PHONY: all clean
all:
ifeq ($(URT_MAKE_KERNEL), y)
	@$(MAKE) --no-print-directory -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)\
		EXTRA_CFLAGS="$(KERNEL_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(KBUILD_EXTRA_SYMBOLS)" modules $(HIGHLIGHT)
else
	@$(SHELL_NOOP)
endif
clean:
ifeq ($(URT_MAKE_KERNEL), y)
	@$(MAKE) --no-print-directory -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
else
	@$(SHELL_NOOP)
endif

endif
