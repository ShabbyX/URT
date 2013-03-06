include ../../Makefile.config
include ../../Makefile.common

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.dep

.PHONY: all clean
all: liburt.a
clean:
	-$(RM) liburt.a *.o

liburt.a: $(LIB_OBJECTS)
	$(AR) $(AR_o) $@ $^
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<
