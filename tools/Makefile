include ../Makefile.config
include ../Makefile.common

TOOLS := urt_names

.PHONY: all
all:
	@$(PRINTF) "$(STATUS_COLOR)Building tools...$(DEFAULT_COLOR)\n"
	@for i in $(TOOLS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) --no-print-directory -C $$i; \
	done $(HIGHLIGHT)
.PHONY: dep
dep:
	@for i in $(TOOLS); do \
		$(MAKE) --no-print-directory -C $$i dep; \
	done
.PHONY: clean
clean:
	@for i in $(TOOLS); do \
		$(MAKE) --no-print-directory -C $$i clean; \
	done
