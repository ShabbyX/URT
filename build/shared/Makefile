include ../../Makefile.config
include ../../Makefile.common

TARGET := liburt-$(URT_VERSION).so

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
clean:
	-$(RM) liburt*.so *.o

URT_BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.src
include ../Makefile.dep

CFLAGS := $(URT_SHARED_CFLAGS)
LDFLAGS := $(URT_SHARED_LDFLAGS)

$(TARGET): $(LIB_OBJ)
	-$(RM) liburt*.so
	$(LD) $(LDFLAGS) $(LD_o) $@ $^
%.o: %.c
	@$(CC) $(CFLAGS) $(CC_o) $@ $< > /dev/null 2>&1
