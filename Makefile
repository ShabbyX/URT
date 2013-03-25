# Targets:
# all: makes the library/kernel module as well as the documentation
# config: generate config headers (done automatically with all)
# doc: makes URT's documentation
# tests: makes tests
# install: installs the software
# uninstall: uninstalls the software

VPATH = include src

.PHONY: all config library doc tests
all: config library tests doc

GENERATED_FILES := urt_config.h urt_internal_config.h urt_version.h

config: $(GENERATED_FILES)
$(GENERATED_FILES): Makefile.config
	@$(MAKE) --no-print-directory -f Makefile.generate all
	@$(RM) build/Makefile.dep tests/*/Makefile.dep
	@$(MAKE) --no-print-directory -C build dep
	@$(MAKE) --no-print-directory -C tests dep

library: $(GENERATED_FILES)
	@$(MAKE) --no-print-directory -C build
doc:
	@$(MAKE) --no-print-directory -C doc
tests:
	@$(MAKE) --no-print-directory -C tests

.PHONY: install uninstall
install uninstall:
	@$(MAKE) --no-print-directory -f Makefile.install $@

.PHONY: clean
clean:
	-@$(MAKE) --no-print-directory -f Makefile.generate clean
	-@$(MAKE) --no-print-directory -C build clean
