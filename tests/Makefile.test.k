# this file is included by */kernel/Makefile

ifdef M
  URT_BASE := $(abspath $(M)/../../..)
else
  URT_BASE := $(abspath $(CURDIR)/../../..)
endif

include $(URT_BASE)/Makefile.config
include $(URT_BASE)/Makefile.common
include $(URT_BASE)/build/Makefile.build

EXTRA_CFLAGS := $(URT_KERNEL_CFLAGS) $(EXTRA_CFLAGS)
KBUILD_EXTRA_SYMBOLS += $(URT_BASE)/build/kernel/Module.symvers

.PHONY: modules clean
modules clean:
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR)\
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(KBUILD_EXTRA_SYMBOLS)" $@

TEST_PARAMS := "$(TARGET)"
ifdef TARGET2
  ifndef COUNT
    COUNT := 1
  endif
  TEST_PARAMS += "$(TARGET2)" "$(COUNT)"
endif
LOAD_KO=
ifeq ($(URT_NEEDS_KO), y)
  LOAD_KO=needs_ko
endif
.PHONY: execute_kernel_test
execute_kernel_test:
	@$(URT_BASE)/tests/execute_test.bash $(LOAD_KO) $(TEST_PARAMS)
