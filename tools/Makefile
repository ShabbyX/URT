include ../Makefile.config
include ../Makefile.common

TOOLS := urt_names urt_clear urt_recover

.PHONY: all
all:
	@$(PRINTF) "$(STATUS_COLOR)Building tools...$(DEFAULT_COLOR)\n"
	@for i in $(TOOLS); do \
		$(PRINTF) "$(SUB_STATUS_COLOR)$$i$(DEFAULT_COLOR)\n"; \
		$(MAKE) -C $$i; \
	done $(HIGHLIGHT)
.PHONY: dep
dep:
	@for i in $(TOOLS); do \
		$(MAKE) -C $$i dep; \
	done
.PHONY: clean
clean:
	@for i in $(TOOLS); do \
		$(MAKE) -C $$i clean; \
	done
