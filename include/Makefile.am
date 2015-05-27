ACLOCAL_AMFLAGS = -I m4

include_HEADERS = \
                  urt_compiler.h \
                  urt_consts.h \
                  urt_lock.h \
                  urt_mem.h \
                  urt_stdtypes.h \
                  urt_task.h \
                  urt_utils.h \
                  urt_defaults.h \
                  urt.h \
                  urt_log.h \
                  urt_setup.h \
                  urt_std.h \
                  urt_time.h \
                  urt_param.h \
                  urt_debug.h

if HAVE_SYS_POSIX
include_HEADERS += \
                   ../sys/posix/include/urt_sys_lock.h \
                   ../sys/posix/include/urt_sys_log.h \
                   ../sys/posix/include/urt_sys_mem.h \
                   ../sys/posix/include/urt_sys_setup.h \
                   ../sys/posix/include/urt_sys_task.h \
                   ../sys/posix/include/urt_sys_time.h \
                   ../sys/posix/include/urt_sys_utils.h
endif

if HAVE_SYS_RTAI
include_HEADERS += \
                   ../sys/rtai/include/urt_sys_rtai.h \
                   ../sys/rtai/include/urt_sys_lock.h \
                   ../sys/rtai/include/urt_sys_log.h \
                   ../sys/rtai/include/urt_sys_mem.h \
                   ../sys/rtai/include/urt_sys_setup.h \
                   ../sys/rtai/include/urt_sys_task.h \
                   ../sys/rtai/include/urt_sys_time.h \
                   ../sys/rtai/include/urt_sys_utils.h
endif
