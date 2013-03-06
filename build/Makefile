include ../Makefile.config
include ../Makefile.common

BASE := $(abspath $(CURDIR)/..)
INC_BUILD := ./increase_build
BUILD_FILE := build.txt

.PHONY: all dep clean
all:
# TODO: proper ifeq for different builds and messages
	@$(PRINTF) "$(STATUS_COLOR)Building URT API...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory -C static $(HIGHLIGHT)
	@$(MAKE) --no-print-directory -C shared $(HIGHLIGHT)
ifeq ($(URT_CONFIG_DEVELOPER_MODE), y)
	@$(MAKE) --no-print-directory $(INC_BUILD)
	@$(MAKE) --no-print-directory $(BUILD_FILE)
endif
clean:
	-$(RM) static/*.o shared/*.o kernel/*.o liburt.a liburt*.so urt.ko

include Makefile.build
dep: Makefile.dep
Makefile.dep: $(LIB_SRC)
	$(CC) $(CFLAGS) $(CC_M) $^ > Makefile.dep

# TODO: target that build_file depends on would be based on what is being built
$(BUILD_FILE): static/liburt.a
	$(INC_BUILD) $@
# TODO: -lrt is probably platform dependent
$(INC_BUILD): increase_build.c
	$(CC_LD) $(CC_o) $@ $< -lrt
