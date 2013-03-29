ifneq ($(KERNELRELEASE),)
  # For older kbuild systems
  include Kbuild
else

ifdef M
  BASE := $(abspath $(M)/../..)
else
  BASE := $(abspath $(CURDIR)/../..)
endif

include $(BASE)/Makefile.config
include $(BASE)/Makefile.common
include $(BASE)/build/Makefile.build
include $(BASE)/build/Makefile.src

.PHONY: all clean
all:
	@$(MAKE) --no-print-directory update_sources
	@$(MAKE) --no-print-directory -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)\
		EXTRA_CFLAGS="$(KERNEL_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(KBUILD_EXTRA_SYMBOLS)" modules $(HIGHLIGHT)
clean:
	@$(MAKE) --no-print-directory -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean

# Note: Currently, you can't build object files in a directory other than the source with kbuild
# So, for now, keep a copy of the source code here.
.PHONY: update_sources
update_sources: $(addprefix $(CURDIR)/,$(LIB_SRC))
	@$(SHELL_NOOP)

$(CURDIR)/%.c: %.c
	@$(CP) '$<' '$@'

endif
