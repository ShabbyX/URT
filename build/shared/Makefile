include ../../Makefile.config
include ../../Makefile.common

TARGET := liburt-$(URT_VERSION).so

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
quiet_cmd_clean = CLEAN   $(CURDIR)/
      cmd_clean = $(RM) liburt*.so *.o
clean:
	-$(call cmd,clean)

URT_BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.src
include ../Makefile.dep

CFLAGS := $(URT_SHARED_CFLAGS)
LDFLAGS := $(URT_SHARED_LDFLAGS)

quiet_cmd_shld = SHLD    $(CURDIR)/$@
      cmd_shld = $(LD) $(LDFLAGS) $(LD_o) $@ $^
quiet_cmd_shcc = SHCC    $(CURDIR)/$@
      cmd_shcc = $(CC) $(CFLAGS) $(CC_o) $@ $<

$(TARGET): $(LIB_OBJ)
	-@$(RM) liburt*.so
	$(call cmd,shld)
%.o: %.c
ifeq ($(URT_CONFIG_DEBUG), y)
	$(call cmd,shcc)
else
	@$(cmd_shcc) > /dev/null 2>&1
endif
