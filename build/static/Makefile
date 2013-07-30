include ../../Makefile.config
include ../../Makefile.common

TARGET := liburt.a

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
quiet_cmd_clean = CLEAN   $(CURDIR)/
      cmd_clean = $(RM) $(TARGET) *.o
clean:
	-$(call cmd,clean)

URT_BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.src
include ../Makefile.dep

CFLAGS := $(URT_STATIC_CFLAGS)
LDFLAGS := $(URT_STATIC_LDFLAGS)

quiet_cmd_ar = AR      $(CURDIR)/$@
      cmd_ar = $(AR) $(AR_o) $@ $^
quiet_cmd_cc = CC      $(CURDIR)/$@
      cmd_cc = $(CC) $(CFLAGS) $(CC_o) $@ $<

$(TARGET): $(LIB_OBJ)
	$(call cmd,ar)
%.o: %.c
	$(call cmd,cc)
