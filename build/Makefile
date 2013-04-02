include ../Makefile.config
include ../Makefile.common

URT_BASE := $(abspath $(CURDIR)/..)
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
ifeq ($(URT_MAKE_USER), y)
	@$(MAKE) --no-print-directory -C static clean
	@$(MAKE) --no-print-directory -C shared clean
endif
ifeq ($(URT_MAKE_KERNEL), y)
	@$(MAKE) --no-print-directory -C kernel clean
	@-$(RM) kernel/*.c
endif
	-$(RM) Makefile.dep
modules:
	@$(PRINTF) "$(STATUS_COLOR)Building URT API...$(DEFAULT_COLOR)\n"
ifeq ($(URT_MAKE_USER), y)
	@$(PRINTF) "$(SUB_STATUS_COLOR)As static library...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory -C static $(HIGHLIGHT)
	@$(PRINTF) "$(SUB_STATUS_COLOR)As shared library...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory -C shared $(HIGHLIGHT)
endif
ifeq ($(URT_MAKE_KERNEL), y)
	@$(PRINTF) "$(SUB_STATUS_COLOR)As kernel module...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory update_kernel_sources
	@$(MAKE) --no-print-directory -C kernel $(HIGHLIGHT)
endif

include Makefile.build
include Makefile.src
dep: Makefile.dep
	@$(SHELL_NOOP)
Makefile.dep: $(LIB_SRC) $(LIB_HDR)
	$(CC) $(CFLAGS) $(CC_M) $(filter-out %.h,$^) > Makefile.dep

ifeq ($(URT_MAKE_KERNEL), y)
# Note: Currently, you can't build object files in a directory other than the source with kbuild
# So, for now, keep a copy of the source code in the kernel folder. I can't put it there because
# then VPATH confuses the sources there with what it should find (even though . is not in VPATH)
.PHONY: update_kernel_sources
update_kernel_sources: $(addprefix $(CURDIR)/kernel/,$(LIB_SRC))
	@$(SHELL_NOOP)

$(CURDIR)/kernel/%.c: %.c
	@$(CP) '$<' '$@'
endif

$(BUILD_FILE): $(INC_BUILD_TARGET)
	$(INC_BUILD) $@
# TODO: -lrt is probably platform dependent
$(INC_BUILD): increase_build.c
	$(CC_LD) $(CC_o) $@ $< -lrt
