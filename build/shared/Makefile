include ../../Makefile.config
include ../../Makefile.common

ifeq ($(URT_MAKE_USER), y)
  TARGET := liburt-$(URT_VERSION).so
else
  TARGET :=
endif

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
clean:
	-$(RM) liburt*.so *.o

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.dep

$(TARGET): $(LIB_OBJ)
	-$(RM) liburt*.so
	$(LD) $(SHARED_LDFLAGS) $(LD_o) $@ $^
%.o: %.c
	@$(CC) $(SHARED_CFLAGS) $(CC_o) $@ $< > /dev/null 2>&1
