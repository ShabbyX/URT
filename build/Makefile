include ../Makefile.config
include ../Makefile.common

BASE := $(abspath $(CURDIR)/..)
INC_BUILD := ./increase_build
BUILD_FILE := build.txt

ifeq ($(URT_MAKE_USER), y)
  INC_BUILD_TARGET := static/liburt.a
else
  INC_BUILD_TARGET := kernel/urt.ko
endif

ifeq ($(URT_CONFIG_DEVELOPER_MODE), y)
  INC_BUILD_DEP := $(INC_BUILD) $(BUILD_FILE)
endif

.PHONY: all modules dep clean
all: dep modules $(INC_BUILD_DEP)
	@$(SHELL_NOOP)
clean:
	@$(MAKE) --no-print-directory -C static clean
	@$(MAKE) --no-print-directory -C shared clean
	@$(MAKE) --no-print-directory -C kernel clean
	-$(RM) Makefile.dep
modules:
	@$(PRINTF) "$(STATUS_COLOR)Building URT API...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory -C static $(HIGHLIGHT)
	@$(MAKE) --no-print-directory -C shared $(HIGHLIGHT)
	@$(MAKE) --no-print-directory -C kernel $(HIGHLIGHT)

include Makefile.build
include Makefile.src
dep: Makefile.dep
	@$(SHELL_NOOP)
Makefile.dep: $(LIB_SRC) $(LIB_HDR)
	$(CC) $(CFLAGS) $(CC_M) $(filter-out %.h,$^) > Makefile.dep

$(BUILD_FILE): $(INC_BUILD_TARGET)
	$(INC_BUILD) $@
# TODO: -lrt is probably platform dependent
$(INC_BUILD): increase_build.c
	$(CC_LD) $(CC_o) $@ $< -lrt
