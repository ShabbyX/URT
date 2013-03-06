include ../../Makefile.config
include ../../Makefile.common

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.dep

.PHONY: all clean
all: liburt-$(URT_VERSION).so
clean:
	-$(RM) liburt*.so *.o

liburt-$(URT_VERSION).so: $(LIB_OBJECTS)
	-$(RM) liburt*.so
	$(LD) $(LDFLAGS) $(LD_o) $@ $^
%.o: %.c
	@$(CC) $(SHARED_CFLAGS) $(CC_o) $@ $< > /dev/null 2>&1
