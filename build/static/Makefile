include ../../Makefile.config
include ../../Makefile.common

TARGET := liburt.a

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
clean:
	-$(RM) $(TARGET) *.o

URT_BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.src
include ../Makefile.dep

CFLAGS := $(URT_STATIC_CFLAGS)
LDFLAGS := $(URT_STATIC_LDFLAGS)

$(TARGET): $(LIB_OBJ)
	$(AR) $(AR_o) $@ $^
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<
