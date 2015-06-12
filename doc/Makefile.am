ACLOCAL_AMFLAGS = -I m4

DT = docthis
DOCDIR = $(top_srcdir)/doc
FILES = \
        generated/html/index.html \
        generated/html/constants.html \
        generated/html/globals.html \
        generated/html/Style.html \
        generated/html/urt.html \
        generated/html/urt_time.html \
        generated/html/urt_task.html \
        generated/html/urt_task_attr.html \
        generated/html/urt_mem.html \
        generated/html/urt_lock.html \
        generated/html/urt_log.html \
        generated/html/urt_utils.html
API_FILES = \
            $(DOCDIR)/urt \
            $(DOCDIR)/urt_time \
            $(DOCDIR)/urt_task \
            $(DOCDIR)/urt_task_attr \
            $(DOCDIR)/urt_mem \
            $(DOCDIR)/urt_lock \
            $(DOCDIR)/urt_log \
            $(DOCDIR)/urt_utils

EXTRA_DIST = $(DOCDIR)/index $(DOCDIR)/constants $(DOCDIR)/globals $(DOCDIR)/Style $(API_FILES) $(DOCDIR)/notices

if HAVE_DOCTHIS
V_DOCTHIS = $(V_DOCTHIS_@AM_V@)
V_DOCTHIS_ = $(V_DOCTHIS_@AM_DEFAULT_V@)
V_DOCTHIS_0 = @echo "  DOCTHIS " $@;
V_DOCTHIS_1 =

all-local:
	@$(MAKE) tex-base-files
	@$(MAKE) files
if HAVE_PDFLATEX
	@$(MAKE) -C generated/tex
endif

.PHONY: tex-base-files
tex-base-files: generated/tex/documentation.tex
	@:
generated/tex/documentation.tex:
	$(V_DOCTHIS)$(DT) +texmain-all $(DOCDIR)/

.PHONY: files
files: $(FILES)
	@:

# Note: I can't find a way around the repetition without using GNU extensions
DT_CMD = $(V_DOCTHIS)$(DT) $< +html +tex +notices $(DOCDIR)/notices
generated/html/index.html: $(DOCDIR)/index $(DOCDIR)/notices
	$(DT_CMD) +css
generated/html/constants.html: $(DOCDIR)/constants $(DOCDIR)/notices
	$(DT_CMD)
generated/html/Style.html: $(DOCDIR)/Style $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt.html: $(DOCDIR)/urt $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_time.html: $(DOCDIR)/urt_time $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_task.html: $(DOCDIR)/urt_task $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_task_attr.html: $(DOCDIR)/urt_task_attr $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_mem.html: $(DOCDIR)/urt_mem $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_lock.html: $(DOCDIR)/urt_lock $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_log.html: $(DOCDIR)/urt_log $(DOCDIR)/notices
	$(DT_CMD)
generated/html/urt_utils.html: $(DOCDIR)/urt_utils $(DOCDIR)/notices
	$(DT_CMD)

generated/html/globals.html: $(DOCDIR)/globals $(API_FILES)
	$(V_DOCTHIS)$(DT) $< +html +tex

clean-local:
	-rm -rf generated
endif
