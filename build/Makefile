include ../Makefile.config
include ../Makefile.common

URT_BASE := $(abspath $(CURDIR)/..)
INC_BUILD := ./increase_build
BUILD_FILE := build.txt

ifeq ($(URT_MAKE_USER), y)
  INC_BUILD_TARGET := static/liburt$(URT_CONFIG_SUFFIX).a
else
  INC_BUILD_TARGET := kernel/urt$(URT_CONFIG_SUFFIX).ko
endif

ifeq ($(URT_CONFIG_DEVELOPER_MODE), y)
  INC_BUILD_DEP := $(INC_BUILD) $(BUILD_FILE)
endif

include Makefile.build
include Makefile.src

.PHONY: all modules dep clean
all: dep modules $(INC_BUILD_DEP)
	@$(SHELL_NOOP)
quiet_cmd_clean = CLEAN   $(CURDIR)/
      cmd_clean = $(RM) Makefile.dep
clean:
ifeq ($(URT_MAKE_USER), y)
	@$(MAKE) -C static clean
	@$(MAKE) -C shared clean
endif
ifeq ($(URT_MAKE_KERNEL), y)
	@$(MAKE) -C kernel clean
	@-$(RM) kernel/*.c
endif
	-$(call cmd,clean)
modules:
	@$(PRINTF) "$(STATUS_COLOR)Building URT API...$(DEFAULT_COLOR)\n"
ifeq ($(URT_MAKE_USER), y)
	@$(PRINTF) "$(SUB_STATUS_COLOR)As static library...$(DEFAULT_COLOR)\n"
	@$(MAKE) -C static $(HIGHLIGHT)
	@$(PRINTF) "$(SUB_STATUS_COLOR)As shared library...$(DEFAULT_COLOR)\n"
	@$(MAKE) -C shared $(HIGHLIGHT)
endif
ifeq ($(URT_MAKE_KERNEL), y)
	@$(PRINTF) "$(SUB_STATUS_COLOR)As kernel module...$(DEFAULT_COLOR)\n"
	@$(MAKE) update_kernel_sources
	@$(MAKE) -C kernel $(HIGHLIGHT)
endif

CFLAGS := $(URT_USER_CFLAGS) $(EXTRA_CFLAGS)
dep: Makefile.dep
	@$(SHELL_NOOP)
quiet_cmd_dep = UPDATE  $(CURDIR)/$@
      cmd_dep = $(CC) $(CFLAGS) $(CC_M) $(filter-out %.h,$^) > $@
Makefile.dep: $(LIB_SRC) $(LIB_HDR)
	$(call cmd,dep)

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

quiet_cmd_inc_build = INC     $(CURDIR)/$@
      cmd_inc_build = $(INC_BUILD) $@
quiet_cmd_cc_ld = CC+LD   $(CURDIR)/$@
      cmd_cc_ld = $(CC_LD) $(CC_o) $@ $< -lrt
# TODO: -lrt is probably platform dependent

$(BUILD_FILE): $(INC_BUILD_TARGET)
	$(call cmd,inc_build)
$(INC_BUILD): increase_build.c
	$(call cmd,cc_ld)
