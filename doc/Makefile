include ../Makefile.config
include ../Makefile.common

DOC_THIS := ../../personal/DocThis/docthis
RM := rm -f
MKDIR := mkdir -p

SOURCE_FILES := urt urt_time urt_task urt_task_attr urt_mem urt_lock urt_log urt_utils
ALL_FILES := $(SOURCE_FILES) index constants Style
HTML_DIR := generated/html
TEX_DIR := generated/tex
HTML_FILES := $(addsuffix .html, $(addprefix $(HTML_DIR)/, $(ALL_FILES)))
TEX_FILES := $(addsuffix .tex, $(addprefix $(TEX_DIR)/, $(ALL_FILES)))
SOURCE_FILES_DEF := $(addprefix src/, $(SOURCE_FILES))
ALL_FILES_DEF := $(addprefix src/, $(ALL_FILES))
TEX_MAKE_FILES := $(addprefix $(TEX_DIR)/, documentation.tex Makefile Makefile.obj Makefile.cvt Makefile.cln)

.PHONY: all files
all:
ifeq ($(URT_CONFIG_BUILD_DOC), y)
	@$(PRINTF) "$(STATUS_COLOR)Building documentation...$(DEFAULT_COLOR)\n"
	@$(MAKE) --no-print-directory files
	$(DOC_THIS) +css
	-$(MKDIR) $(HTML_DIR)/files
	-$(MKDIR) $(TEX_DIR)/files
	@$(MAKE) --no-print-directory -C generated/tex
else
	@$(SHELL_NOOP)
endif
files: $(TEX_MAKE_FILES) $(HTML_FILES) $(HTML_DIR)/globals.html $(TEX_FILES) $(TEX_DIR)/globals.tex
	@$(SHELL_NOOP)
$(HTML_DIR)/%.html $(TEX_DIR)/%.tex: src/%
	$(DOC_THIS) $< +html +tex
$(HTML_DIR)/globals.html $(TEX_DIR)/globals.tex: src/globals $(SOURCE_FILES_DEF)
	$(DOC_THIS) $< +html +tex
$(TEX_MAKE_FILES):
	$(DOC_THIS) +texmain-all src
.PHONY: clean
clean:
	-$(RM) $(HTML_FILES) $(HTML_DIR)/globals.html
	-$(RM) $(TEX_FILES) $(TEX_DIR)/globals.tex
	-@$(MAKE) --no-print-directory -C generated/tex clean
	-$(RM) $(TEX_MAKE_FILES)
