ACLOCAL_AMFLAGS = -I m4

if HAVE_USER
TOOL_CFLAGS = $(URT_CFLAGS_USER) -I$(top_srcdir)/include -I$(top_srcdir)/sys/$(URT_SYS)/include
TOOL_LDADD = ../../src/liburt@URT_SUFFIX@.la $(URT_LDFLAGS_USER)
endif
