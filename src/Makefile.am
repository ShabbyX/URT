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
               task_common.c \
               inline_common.c

HEADER_FILES = \
               urt_internal.h \
               urt_reserved.h

if HAVE_SYS_POSIX
  SOURCE_FILES += \
                  ../sys/posix/src/lock.c \
                  ../sys/posix/src/mem.c \
                  ../sys/posix/src/setup.c \
                  ../sys/posix/src/task.c \
                  ../sys/posix/src/inline.c \
                  ../sys/posix/src/names.c
  HEADER_FILES += \
                  ../sys/posix/include/urt_sys_internal.h \
                  ../sys/posix/src/mem_internal.h \
                  ../sys/posix/src/names.h \
                  ../sys/posix/src/time_internal.h
endif

if HAVE_SYS_RTAI
  SOURCE_FILES += \
                  ../sys/rtai/src/lock.c \
                  ../sys/rtai/src/mem.c \
                  ../sys/rtai/src/setup.c \
                  ../sys/rtai/src/task.c \
                  ../sys/rtai/src/inline.c
  HEADER_FILES += \
                  ../sys/rtai/include/urt_sys_internal.h \
                  ../sys/rtai/include/urt_sys_rtai.h
endif

if HAVE_USER
lib_LTLIBRARIES = liburt@URT_SUFFIX@.la
liburt@URT_SUFFIX@_la_SOURCES = $(SOURCE_FILES) $(HEADER_FILES)
liburt@URT_SUFFIX@_la_CFLAGS = $(URT_CFLAGS_USER) -I"$(top_srcdir)/include" -I"$(top_srcdir)/sys/$(URT_SYS)/include"
liburt@URT_SUFFIX@_la_LDFLAGS = -version-info 0:0:0
endif

if HAVE_KERNEL
KBUILD_EXTRA_CFLAGS = \
                      -I"$(abs_top_srcdir)/src" \
		      -I"$(abs_top_srcdir)/sys/$(URT_SYS)/src" \
		      $(KBUILD_CFLAGS)
KBUILD_EXTRA_SYMBOLS = $(KBUILD_SYMBOLS)

V_KERN_INVOKE = $(V_KERN_INVOKE_@AM_V@)
V_KERN_INVOKE_ = $(V_KERN_INVOKE_@AM_DEFAULT_V@)
V_KERN_INVOKE_0 = @
V_KERN_INVOKE_1 =

all-local:
	$(V_KERN_INVOKE)$(URT_KERNEL_INVOKE) modules
clean-local:
	$(V_KERN_INVOKE)$(URT_KERNEL_INVOKE) clean
	rm -f "$(srcdir)"/*.o "$(srcdir)"/.*.cmd
install-exec-local:
	$(V_KERN_INVOKE)$(URT_KERNEL_INVOKE) modules_install
	cp Module.symvers $(DESTDIR)$(URT_KERNEL_MODULE_PATH)
endif
