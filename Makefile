# Targets:
# all: makes the library/kernel module as well as the documentation
# config: generate config headers (done automatically with all)
# doc: makes URT's documentation
# install: installs the software
# uninstall: uninstalls the software

VPATH = include

.PHONY: all config library doc
all: config library doc

config: urt_config.h
urt_config.h: Makefile.config
	@$(MAKE) --no-print-directory -f Makefile.generate all

library:
	@$(MAKE) --no-print-directory -C src
doc:
	@$(MAKE) --no-print-directory -C doc

.PHONY: install uninstall
install uninstall:
	@$(MAKE) --no-print-directory -f Makefile.install $@

.PHONY: clean
clean:
	-@$(MAKE) --no-print-directory -f Makefile.generate clean
	-@$(MAKE) --no-print-directory -C src clean
	#-@$(MAKE) --no-print-directory -C doc clean
