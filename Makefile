# Targets:
# all: makes the library/kernel module as well as the documentation
# config: generate config headers (done automatically with all)
# doc: makes URT's documentation
# tests: makes tests
# install: installs the software
# uninstall: uninstalls the software

-include Makefile.config

ifneq ($(URT_CONFIG_VERBOSE_BUILD), y)
  MAKEFLAGS := $(MAKEFLAGS) --no-print-directory
endif

.PHONY: all config library tools tests doc
all: config library tools tests doc

config: Makefile.common
Makefile.common: Makefile.config Makefile.generate
	@$(MAKE) -f Makefile.generate all
	@$(RM) build/Makefile.dep tests/*/Makefile.dep
	@$(MAKE) -C build dep
	@$(MAKE) -C tools dep
	@$(MAKE) -C tests dep
	@touch Makefile.common

library: config
	@$(MAKE) -C build
doc:
	@$(MAKE) -C doc
tools: library config
	@$(MAKE) -C tools
tests: library config
	@$(MAKE) -C tests

.PHONY: check
check:
	@$(MAKE) -C tests user
	@$(MAKE) -C tests valgrind
	@$(MAKE) -C tests kernel

.PHONY: install uninstall
install uninstall:
	@$(MAKE) -f Makefile.install $@

.PHONY: clean
clean:
	-@$(MAKE) -C build clean
	-@$(MAKE) -C tests clean
	-@$(MAKE) -C tools clean
