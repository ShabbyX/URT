include ../../Makefile.config
include ../../Makefile.common

.PHONY: all clean
all: liburt.a
	@$(SHELL_NOOP)
clean:
	-$(RM) liburt.a *.o

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.dep

liburt.a: $(LIB_OBJ)
	$(AR) $(AR_o) $@ $^
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<
