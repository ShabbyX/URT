ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

ifdef M
  URT_BASE := $(abspath $(M)/../..)
else
  URT_BASE := $(abspath $(CURDIR)/../..)
endif

include $(URT_BASE)/Makefile.config
include $(URT_BASE)/Makefile.common
include $(URT_BASE)/build/Makefile.build
include $(URT_BASE)/build/Makefile.src

.PHONY: all clean
all:
	@-$(RM) urt*.ko
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)\
		EXTRA_CFLAGS="$(URT_KERNEL_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(KBUILD_EXTRA_SYMBOLS)" modules
clean:
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean

endif
