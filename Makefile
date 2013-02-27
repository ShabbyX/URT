# Targets:
# all: makes the library/kernel module as well as the documentation
# doc: makes URT's documentation
# install: installs the software
# uninstall: uninstalls the software

.PHONY: all library doc
all: library doc

library:
	@$(MAKE) --no-print-directory -C src
doc:
	@$(MAKE) --no-print-directory -C doc

.PHONY: install uninstall
install uninstall:
	@$(MAKE) --no-print-directory -f Makefile.install $@

.PHONY: clean
clean:
	-@$(MAKE) --no-print-directory -C src clean
	#-@$(MAKE) --no-print-directory -C doc clean
