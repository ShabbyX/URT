include ../Makefile.config
include ../Makefile.common

BASE := $(abspath $(CURDIR)/..)
INC_BUILD := ./increase_build
BUILD_FILE := build.txt

ifeq ($(URT_CONFIG_DEVELOPER_MODE), y)
  INC_BUILD_DEP := $(INC_BUILD) $(BUILD_FILE)
endif

.PHONY: all modules dep clean
all: modules $(INC_BUILD_DEP)
	@$(SHELL_NOOP)
clean:
	@$(MAKE) --no-print-directory -C static clean
	@$(MAKE) --no-print-directory -C shared clean
	@$(MAKE) --no-print-directory -C kernel clean
	-$(RM) Makefile.dep
modules:
# TODO: proper ifeq for different builds and messages
	@$(PRINTF) "$(STATUS_COLOR)Building URT API...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory -C static $(HIGHLIGHT)
	@$(MAKE) --no-print-directory -C shared $(HIGHLIGHT)

include Makefile.build
dep: Makefile.dep
	@$(SHELL_NOOP)
Makefile.dep: $(LIB_SRC) $(LIB_HDR)
	$(CC) $(CFLAGS) $(CC_M) $^ > Makefile.dep

# TODO: target that build_file depends on would be based on what is being built
$(BUILD_FILE): static/liburt.a
	$(INC_BUILD) $@
# TODO: -lrt is probably platform dependent
$(INC_BUILD): increase_build.c
	$(CC_LD) $(CC_o) $@ $< -lrt
