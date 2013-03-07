include ../Makefile.config
include ../Makefile.common

TESTS := setup

.PHONY: all
all:
	@$(PRINTF) "$(STATUS_COLOR)Building tests...$(DEFAULT_COLOR)\n"
	@for i in $(TESTS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) --no-print-directory -C $$i; \
	done
.PHONY: dep
dep:
	@for i in $(TESTS); do \
		$(MAKE) --no-print-directory -C $$i dep; \
	done
.PHONY: tests valgrind
tests:
	@$(PRINTF) "$(STATUS_COLOR)Executing tests...$(DEFAULT_COLOR)\n"
	@for i in $(TESTS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) --no-print-directory -C $$i execute_test || exit 1; \
		$(PRINTF) "-- passed"; \
	done
valgrind:
	@$(PRINTF) "$(STATUS_COLOR)Executing valgrind tests...$(DEFAULT_COLOR)\n"
	@if command -v valgrind > /dev/null 2>&1; then \
		for i in $(TESTS); do \
			$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
			$(MAKE) --no-print-directory -C $$i valgrind_check; \
		done \
	else \
		$(PRINTF) "Valgrind not installed\n"; \
	fi
.PHONY: clean
clean:
	@for i in $(TESTS); do \
		$(MAKE) --no-print-directory -C $$i clean; \
	done
