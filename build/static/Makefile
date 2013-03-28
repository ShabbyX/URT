include ../../Makefile.config
include ../../Makefile.common

ifeq ($(URT_MAKE_USER), y)
  TARGET := liburt.a
else
  TARGET :=
endif

.PHONY: all clean
all: $(TARGET)
	@$(SHELL_NOOP)
clean:
	-$(RM) $(TARGET) *.o

BASE := $(abspath $(CURDIR)/../..)
include ../Makefile.build
include ../Makefile.src
include ../Makefile.dep
LIB_OBJ := $(LIB_SRC:.c=.o)

$(TARGET): $(LIB_OBJ)
	$(AR) $(AR_o) $@ $^
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<
