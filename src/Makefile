include ../Makefile.config
include ../Makefile.common

CFLAGS += -I../include -I../sys/$(URT_SYS_DIR)/include $(URT_CFLAGS) $(EXTRA_CFLAGS)
SHARED_CFLAGS += -I../include -I../sys/$(URT_SYS_DIR)/include $(URT_SHARED_CFLAGS) $(EXTRA_CFLAGS)
LDFLAGS += $(URT_LDFLAGS) $(EXTRA_LDFLAGS)
INC_BUILD := ./increase_build
BUILD_FILE := build.txt
BUILD_DIR := build
STATIC_BUILD_DIR := build/static
SHARED_BUILD_DIR := build/shared

VPATH = ../sys/$(URT_SYS_DIR)/src

.PHONY: all
all: api
.PHONY: api
api:
# TODO: proper ifeq for different builds and messages
	@$(PRINTF) "$(STATUS_COLOR)Building URT API...$(DEFAULT_COLOR)\n"
	-@$(MKDIR) $(BUILD_DIR) > /dev/null 2>&1 || exit 0
	-@$(MKDIR) $(STATIC_BUILD_DIR) > /dev/null 2>&1 || exit 0
	-@$(MKDIR) $(SHARED_BUILD_DIR) > /dev/null 2>&1 || exit 0
	@$(MAKE) --no-print-directory liburt.a $(HIGHLIGHT)
	@$(MAKE) --no-print-directory liburt-$(URT_VERSION).so $(HIGHLIGHT)
ifeq ($(URT_CONFIG_DEVELOPER_MODE), y)
	@$(MAKE) --no-print-directory $(INC_BUILD)
	@$(MAKE) --no-print-directory $(BUILD_FILE)
endif

LIB_SRC := setup.c registry.c
include ../sys/$(URT_SYS_DIR)/Makefile.src
LIB_OBJECTS := $(LIB_SRC:.c=.o)

liburt.a: $(addprefix $(STATIC_BUILD_DIR)/, $(LIB_OBJECTS))
	$(AR) $(AR_o) $@ $^

liburt-$(URT_VERSION).so: $(addprefix $(SHARED_BUILD_DIR)/, $(LIB_OBJECTS))
	-$(RM) liburt*.so
	$(LD) $(LDFLAGS) $(LD_o) $@ $^

# TODO: make a rule that generates dependencies from $(LIB_SRC)
$(STATIC_BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<
$(SHARED_BUILD_DIR)/%.o: %.c
	@$(CC) $(SHARED_CFLAGS) $(CC_o) $@ $< > /dev/null 2>&1

$(BUILD_FILE): liburt.a
	$(INC_BUILD) $@
# TODO: -lrt is probably platform dependent
$(INC_BUILD): increase_build.c
	$(CC) $(CC_o) $@ $< -lrt

.PHONY: clean
clean:
	-$(RM) $(BUILD_DIR)/*.o
	-$(RM) liburt.a liburt*.so
	-$(RM) $(INC_BUILD)
