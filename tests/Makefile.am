ACLOCAL_AMFLAGS = -I m4

SUBDIRS = setup task mutex sem mem cond rwlock

if HAVE_PYTHON
  SUBDIRS += python
endif

check_SCRIPTS = execute_test.sh
