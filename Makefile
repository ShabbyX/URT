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

VPATH := include:src

.PHONY: all config library tools tests doc
all: config library tools tests doc

config: Makefile.common
Makefile.common: Makefile.config
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
tools: config
	@$(MAKE) -C tools
tests: config
	@$(MAKE) -C tests

.PHONY: check
check:
	@$(MAKE) -C tests tests
ifneq ($(URT_CONFIG_RT_SUBSYSTEM), rtai)
  ifneq ($(URT_CONFIG_RT_SUBSYSTEM), rtai-kernel-only)
    ifneq ($(URT_CONFIG_RT_SUBSYSTEM), rtai-user-only)
	@$(MAKE) -C tests valgrind
    endif
  endif
endif

.PHONY: install uninstall
install uninstall:
	@$(MAKE) -f Makefile.install $@

.PHONY: clean
clean:
	-@$(MAKE) -C build clean
	-@$(MAKE) -C tests clean
	-@$(MAKE) -C tools clean
