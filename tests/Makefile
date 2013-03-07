include ../Makefile.config
include ../Makefile.common

TESTS := setup sem mem

.PHONY: all
all:
	@for i in $(TESTS); do \
		$(ECHO) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)"; \
		$(MAKE) --no-print-directory -C $$i; \
	done
.PHONY: dep
dep:
	@for i in $(TESTS); do \
		$(MAKE) --no-print-directory -C $$i dep; \
	done
.PHONY: tests valgrind
tests:
	@for i in $(TESTS); do \
		$(ECHO) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)"; \
		$(MAKE) --no-print-directory -C $$i execute_test || exit 1; \
		$(ECHO) "-- passed"; \
	done
valgrind:
	@if command -v valgrind > /dev/null 2>&1; then \
		for i in $(TESTS); do \
			$(ECHO) "$(SUB_STATUS_COLOR)$$i (valgrind)$(DEFAULT_COLOR)"; \
			$(MAKE) --no-print-directory -C $$i valgrind_check; \
		done \
	else \
		$(ECHO) Valgrind not installed; \
	fi
.PHONY: clean
clean:
	@for i in $(TESTS); do \
		$(MAKE) --no-print-directory -C $$i clean; \
	done
