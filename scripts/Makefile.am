ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = urt@URT_SUFFIX@-config

install-exec-local:
	-$(MKDIR_P) $(DESTDIR)/etc/bash_completion.d/
	-$(INSTALL_SCRIPT) urt@URT_SUFFIX@-config-bc $(DESTDIR)/etc/bash_completion.d/

uninstall-local:
	-rm -f $(DESTDIR)/etc/bash_completion.d/urt@URT_SUFFIX@-config-bc
