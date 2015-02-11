AC_PREREQ([2.68])
AC_INIT([URT], [0.4.2], [https://github.com/ShabbyX/URT/issues])
AC_CONFIG_AUX_DIR([bin])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([1.14 -Wall -Werror foreign subdir-objects])

AC_CONFIG_SRCDIR([src/registry.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs
AC_PROG_CC_C99
AC_PROG_INSTALL
AM_PROG_AR
AC_PROG_MKDIR_P

LT_INIT

# Make sure C99 is available
AS_IF([test x"$ac_cv_prog_cc_c99" = xno],
  [AC_MSG_ERROR([C99 support is required])])

# Check the real-time back-end.  One of the following
#
#     posix:            Linux (non-realtime) (alpha version), QNX (Noy yet implemented), VxWorks (Not yet implemented)
#     rtai:             Real-time library and kernel module for Linux  (In implementation)
#     xenomai:          Real-time library for Linux (Not yet implemented)
#     rtlinux:          Real-time patches (RT-Linux) for the Linux kernel (Not yet implemented)
#
AC_MSG_CHECKING([real-time back-end to use])
backend=posix
AC_ARG_WITH(back-end,
            [AS_HELP_STRING([--with-back-end=<rt-system>], [One of {posix, rtai}])],
            [AS_CASE(["$withval"],
              ["" | y | ye | yes | n | no], [AC_MSG_ERROR([Missing argument to --with-back-end.  See --help])],
              [posix | Posix | POSIX], [backend=posix],
              [rtai | Rtai | RTAI], [backend=rtai],
              [xenomai | Xenomai | XENOMAI], [AC_MSG_ERROR([Sorry, XENOMAI is not yet supported])],
              [rtlinux | Rtlinux | RTlinux | RTLinux | RTLINUX], [AC_MSG_ERROR([Sorry, RTLINUX is not yet supported])],
              [*], [AC_MSG_ERROR([Unknown real-time back-end $witval.  See --help])]
            )])
AC_MSG_RESULT([$backend])

# Check for back-end specific requirements
headers_to_check="stddef.h stdlib.h string.h ctype.h signal.h stdint.h stdbool.h"
headers_to_check="$headers_to_check unistd.h fcntl.h sys/stat.h sys/types.h sys/mman.h semaphore.h pthread.h"
rtai_config_tool=rtai-config
build_user_lib=y
build_kernel_module=n
AS_CASE(["$backend"],
  [posix], [],
  [rtai], [AC_MSG_CHECKING([for working rtai-config])
           SH_GET_CONFIG_STR(rtai-config, rtai_config_tool, [Path to rtai-config (only if back-end=rtai)], [rtai-config])
           # Make sure the given rtai-config actually works
           AS_IF([$rtai_config_tool --version > /dev/null 2>&1],
             [AC_MSG_RESULT([$rtai_config_tool])],
             [AC_MSG_ERROR(["$rtai_config_tool" did not work])])
           build_kernel_module=y])

AS_IF([test x"$headers_to_check" != x""],
  [AC_CHECK_HEADERS([$headers_to_check], [], [AC_MSG_ERROR(Required header missing)])])

# Check for back-end specific flags
extra_cflags_user=
extra_ldflags_user=
extra_cflags_kernel=
extra_symbols_kernel=
tests_need_urt_ko=n
AS_CASE(["$backend"],
  [posix], [AS_CASE(["$host_os"],
             [*linux*], [AC_CHECK_LIB([rt], [clock_gettime])
                         extra_cflags_user="-pthread"
                         extra_ldflags_user="-lpthread"])],
  [rtai], [extra_cflags_user=$($rtai_config_tool --lxrt-cflags)
           extra_ldflags_user=$($rtai_config_tool --lxrt-ldflags)
           extra_cflags_kernel=$($rtai_config_tool --module-cflags)
           extra_symbols_kernel=$($rtai_config_tool --module-dir)/Module.symvers
           tests_need_urt_ko=y]
)

# Checks for typedefs, structures, and compiler characteristics
AS_IF([test x"$build_user_lib" = xy],
  [AC_C_INLINE
   AC_TYPE_SIZE_T])

# Checks for library functions
AS_IF([test x"$build_user_lib" = xy],
  [functions_to_check="malloc"
   AS_CASE(["$backend"],
     [posix], [AS_VAR_APPEND(functions_to_check, " realloc mmap munmap ftruncate memset strchr strerror")],
     [rtai], []
   )])

AS_IF([test x"$functions_to_check" != x""],
  [AC_CHECK_FUNCS([$functions_to_check], [], [AC_MSG_ERROR(Required function missing)])])

# Check for URT suffix
# A suffix is useful for having multiple URT installations.  Esentially, everywhere a file name has
# urt in it, it becomes urt$suffix
suffix=
SH_GET_CONFIG_STR(suffix, suffix, [A suffix to append to urt in every file name], "")

# Set up a short way of invoking kernel Makefile for module builds
AS_IF([test x"$build_kernel_module" = xy],
  [kernel_path=/lib/modules/$(uname -r)
   kernel_build_path=$kernel_path/build
   kernel_module_path=$kernel_path/urt"$suffix"
   kernel_invoke="\$(MAKE) -C '$kernel_build_path' M='\$(abs_builddir)' V=\$(V) \
EXTRA_CFLAGS='-I\"\$(abs_top_srcdir)/include\" -I\"\$(abs_top_srcdir)/sys/\$(URT_SYS)/include\" $extra_cflags_kernel \$(KBUILD_EXTRA_CFLAGS)' \
KBUILD_EXTRA_SYMBOLS='$extra_symbols_kernel \$(KBUILD_EXTRA_SYMBOLS)' \
INSTALL_MOD_DIR=urt'$suffix' INSTALL_MOD_PATH=\$(DESTDIR)"])

# Check for URT parameters
max_shared_objects=4096
lock_stop_max_delay=10000000
default_stack_size=4096
SH_GET_CONFIG_NUM(max-shared-objects, max_shared_objects, [Maximum number of shared objects], 4096)
SH_GET_CONFIG_NUM(lock-stop-max-delay, lock_stop_max_delay, [Maximum delay for lock to respond to cancellation.
                                                             Higher values decrease responsiveness in such cases,
                                                             but decrease processor usage.  Too low values would
                                                             result in busy-waiting], 10000000)
SH_GET_CONFIG_NUM(default-stack-size, default_stack_size, [Default stack size to request when not specified], 4096)

# Check for valgrind
AS_IF([test x"$build_user_lib" = xy && test x"$backend" != xrtai],
  [AC_CHECK_PROG(VALGRIND, valgrind, y)])

# Check for DocThis!
AC_CHECK_PROG(DOCTHIS, docthis, y)

# Check for pdflatex
AC_CHECK_PROG(PDFLATEX, pdflatex, y)

# Configuration
bi_space=0
AS_CASE(["$backend"],
  [posix], [],
  [rtai], [bi_space=1]
)
AC_DEFINE_UNQUOTED(URT_MAX_OBJECTS, [$max_shared_objects], [maximum possible number of shared objects])
AC_DEFINE_UNQUOTED(URT_LOCK_STOP_MAX_DELAY, [$lock_stop_max_delay], [maximum delay for locks to respond to cancelation])
AC_DEFINE_UNQUOTED(URT_DEFAULT_STACK_SIZE, [$default_stack_size], [the default stack size for tasks])
AC_DEFINE_UNQUOTED(URT_SUFFIX, ["$suffix"], [the suffix used to build URT with])
AC_DEFINE_UNQUOTED(URT_BI_SPACE, [$bi_space], [whether URT is runnable on both user and kernel spaces])

# Build
AC_SUBST(URT_SYS, [$backend])
AS_IF([test x"$backend" = xrtai],
  [AC_SUBST(URT_RTAI_CONFIG, [$rtai_config_tool])])
AC_SUBST(URT_BUILD_USER, [$build_user_lib])
AC_SUBST(URT_BUILD_KERNEL, [$build_kernel_module])
AC_SUBST(URT_CFLAGS_USER, [$extra_cflags_user])
AC_SUBST(URT_LDFLAGS_USER, [$extra_ldflags_user])
AC_SUBST(URT_CFLAGS_KERNEL, [$extra_cflags_kernel])
AC_SUBST(URT_SYMBOLS_KERNEL, [$extra_symbols_kernel])
AC_SUBST(URT_VERSION, [$PACKAGE_VERSION])
AC_SUBST(URT_SUFFIX, [$suffix])
AC_SUBST(URT_SUFFIX_NOWS, [${suffix//@<:@@<:@:space:@:>@@:>@/}])
AC_SUBST(URT_KERNEL_INVOKE, [$kernel_invoke])
AC_SUBST(URT_KERNEL_MODULE_PATH, [$kernel_module_path])
AC_SUBST(URT_TESTS_NEED_URT_KO, [$tests_need_urt_ko])

# Optional build
AM_CONDITIONAL(HAVE_USER, [test x"$build_user_lib" = xy])
AM_CONDITIONAL(HAVE_KERNEL, [test x"$build_kernel_module" = xy])
AM_CONDITIONAL(HAVE_TOOLS, [test x"$build_user_lib" = xy])
AM_CONDITIONAL(HAVE_VALGRIND, [test x"$VALGRIND" = xy])
AM_CONDITIONAL(HAVE_DOCTHIS, [test x"$DOCTHIS" = xy])
AM_CONDITIONAL(HAVE_PDFLATEX, [test x"$PDFLATEX" = xy])
AM_CONDITIONAL(HAVE_SYS_POSIX, [test x"$backend" = xposix])
AM_CONDITIONAL(HAVE_SYS_RTAI, [test x"$backend" = xrtai])
AM_CONDITIONAL(NEED_ROOT, [test x"$backend" = xrtai])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 src/Makefile
                 include/Makefile
                 tests/Makefile
                 tests/cond/Makefile
                 tests/mem/Makefile
                 tests/mutex/Makefile
                 tests/rwlock/Makefile
                 tests/sem/Makefile
                 tests/setup/Makefile
                 tests/task/Makefile
                 scripts/Makefile])

AS_IF([test x"$build_user_lib" = xy],
  [AC_CONFIG_FILES([tools/Makefile
                    tools/clear/Makefile
                    tools/dump/Makefile
                    tools/names/Makefile
                    tools/recover/Makefile])])

AS_IF([test x"$build_kernel_module" = xy],
  [AC_CONFIG_FILES([src/Kbuild
                    tests/cond/Kbuild
                    tests/mem/Kbuild
                    tests/mutex/Kbuild
                    tests/rwlock/Kbuild
                    tests/sem/Kbuild
                    tests/setup/Kbuild
                    tests/task/Kbuild])])

AC_CONFIG_FILES([scripts/urt${suffix}-config:scripts/urt-config.in],
                [chmod +x scripts/urt${suffix}-config])
AC_CONFIG_FILES([scripts/urt${suffix}-config-bc:scripts/urt-config_bash_completion.in])

AC_OUTPUT
