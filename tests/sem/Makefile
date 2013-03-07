include ../../Makefile.config
include ../../Makefile.common

BASE := $(abspath $(CURDIR)/../..)
include $(BASE)/build/Makefile.build
#LDFLAGS += -L$(BASE)/build/shared -lurt-$(URT_VERSION)
LDFLAGS := -L$(BASE)/build/shared -lurt-$(URT_VERSION) $(LDFLAGS)
TARGET := test
TARGET2 := wait

.PHONY: all dep clean
all: $(TARGET) $(TARGET2)
clean:
	-$(RM) $(TARGET) $(TARGET2) *.o Makefile.dep
dep: Makefile.dep
Makefile.dep: main.c wait.c
	$(CC) $(CFLAGS) $(CC_M) $^ > Makefile.dep

$(TARGET): main.o $(BASE)/build/shared/liburt-$(URT_VERSION).so
	$(LD) $(LD_o) $@ $(filter-out %.so,$^) $(LDFLAGS)
$(TARGET2): wait.o $(BASE)/build/shared/liburt-$(URT_VERSION).so
	$(LD) $(LD_o) $@ $(filter-out %.so,$^) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<

-include Makefile.dep
