ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = Kbuild

SOURCE_FILES = \
               kern_main.c \
               lock_common.c \
               mem_common.c \
               new_common.c \
               param.c \
               registry.c \
               setup_common.c \
               task_common.c

SOURCE_FILES += \
               ../sys/@URT_SYS@/src/lock.c \
               ../sys/@URT_SYS@/src/mem.c \
               ../sys/@URT_SYS@/src/setup.c \
               ../sys/@URT_SYS@/src/task.c

HEADER_FILES = \
               urt_internal.h \
               urt_reserved.h

if HAVE_SYS_POSIX
  SOURCE_FILES += \
                  ../sys/@URT_SYS@/src/names.c
  HEADER_FILES += \
                  ../sys/@URT_SYS@/src/mem_internal.h \
                  ../sys/@URT_SYS@/src/names.h \
                  ../sys/@URT_SYS@/src/time_internal.h
endif

if HAVE_SYS_RTAI
  SOURCE_FILES += \
                  ../sys/@URT_SYS@/src/utils.c
endif

if HAVE_USER
lib_LTLIBRARIES = liburt@URT_SUFFIX@.la
liburt@URT_SUFFIX@_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)
liburt@URT_SUFFIX@_la_CFLAGS = $(URT_CFLAGS_USER) -I$(top_srcdir)/include -I$(top_srcdir)/sys/$(URT_SYS)/include
liburt@URT_SUFFIX@_la_LDFLAGS = -version-info 0:0:0
endif

if HAVE_KERNEL
KBUILD_EXTRA_CFLAGS = \
                      -I$(abs_top_srcdir)/src \
                      -I$(abs_top_srcdir)/sys/$(URT_SYS)/src
KBUILD_EXTRA_SYMBOLS =

all-local:
	$(URT_KERNEL_INVOKE) modules
clean-local:
	$(URT_KERNEL_INVOKE) clean
	rm -f "$(srcdir)"/*.o "$(srcdir)"/.*.cmd
install-exec-local:
	$(URT_KERNEL_INVOKE) modules_install
	cp Module.symvers $(URT_KERNEL_MODULE_PATH)
endif
