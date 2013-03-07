include ../../Makefile.config
include ../../Makefile.common

BASE := $(abspath $(CURDIR)/../..)
include $(BASE)/build/Makefile.build
#LDFLAGS += -L$(BASE)/build/shared -lurt-$(URT_VERSION)
LDFLAGS := -L$(BASE)/build/static -lurt $(LDFLAGS)
TARGET := test

.PHONY: all dep clean
all: $(TARGET)
clean:
	-$(RM) $(TARGET) *.o Makefile.dep
dep: Makefile.dep
Makefile.dep: main.c
	$(CC) $(CFLAGS) $(CC_M) $^ > Makefile.dep

$(TARGET): main.o $(BASE)/build/static/liburt.a
	$(LD) $(LD_o) $(TARGET) $(filter-out %.a,$^) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<

-include Makefile.dep
