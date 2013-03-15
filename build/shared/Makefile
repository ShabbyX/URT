include ../../Makefile.config
include ../../Makefile.common

.PHONY: all clean
all: liburt-$(URT_VERSION).so
	@$(SHELL_NOOP)
clean:
	-$(RM) liburt*.so *.o

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.dep

liburt-$(URT_VERSION).so: $(LIB_OBJ)
	-$(RM) liburt*.so
	$(LD) $(SHARED_LDFLAGS) $(LD_o) $@ $^
%.o: %.c
	@$(CC) $(SHARED_CFLAGS) $(CC_o) $@ $< > /dev/null 2>&1
