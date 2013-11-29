include ../Makefile.config
include ../Makefile.common

TESTS := setup sem mem rwlock task mutex

ifneq ($(URT_CONFIG_BUILD_TESTS), y)
.PHONY: all dep user valgrind kernel clean
all dep user valgrind kernel clean:
	@$(SHELL_NOOP)
else
.PHONY: all
all:
	@$(PRINTF) "$(STATUS_COLOR)Building tests...$(DEFAULT_COLOR)\n"
	@for i in $(TESTS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) -C $$i; \
	done $(HIGHLIGHT)
.PHONY: dep
dep:
	@for i in $(TESTS); do \
		$(MAKE) -C $$i dep; \
	done
ifeq ($(URT_MAKE_USER), y)
.PHONY: user
user:
	@$(PRINTF) "$(STATUS_COLOR)Executing user tests...$(DEFAULT_COLOR)\n"
	@for i in $(TESTS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) -C $$i execute_user_test || exit 1; \
		$(PRINTF) "-- passed\n"; \
	done
endif
ifeq ($(URT_CAN_VALGRIND), y)
.PHONY: valgrind
valgrind:
	@$(PRINTF) "$(STATUS_COLOR)Executing valgrind tests...$(DEFAULT_COLOR)\n"
	@if command -v valgrind > /dev/null 2>&1; then \
		for i in $(TESTS); do \
			$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
			$(MAKE) -C $$i valgrind_check; \
		done \
	else \
		$(PRINTF) "Valgrind not installed\n"; \
	fi
endif
ifeq ($(URT_MAKE_KERNEL), y)
.PHONY: kernel
kernel:
	@$(PRINTF) "$(STATUS_COLOR)Executing kernel tests...$(DEFAULT_COLOR)\n"
	@for i in $(TESTS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) -C $$i/kernel execute_kernel_test; \
	done
endif
.PHONY: clean
clean:
	@for i in $(TESTS); do \
		$(MAKE) -C $$i clean; \
	done
endif
