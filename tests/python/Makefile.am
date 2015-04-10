ACLOCAL_AMFLAGS = -I m4

AM_TESTS_ENVIRONMENT = \
python="$(PYTHON)"; export python; \
needs_ko="$(URT_TESTS_NEED_URT_KO)"; export needs_ko; \
top_builddir="$(top_builddir)"; export top_builddir;
if NEED_ROOT
  AM_TESTS_ENVIRONMENT += \
  needs_root=y; export needs_root;
endif

TESTS = run.sh

check_SCRIPTS = $(TESTS)

EXTRA_DIST = test.py $(check_SCRIPTS)
