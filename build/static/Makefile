include ../../Makefile.config
include ../../Makefile.common

TARGET := liburt.a

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
clean:
	-$(RM) $(TARGET) *.o

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.src
include ../Makefile.dep

$(TARGET): $(LIB_OBJ)
	$(AR) $(AR_o) $@ $^
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<
