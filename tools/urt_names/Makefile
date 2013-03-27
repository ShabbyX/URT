include ../../Makefile.config
include ../../Makefile.common

BASE := $(abspath $(CURDIR)/../..)
include $(BASE)/build/Makefile.build
LDFLAGS := -L$(BASE)/build/shared -lurt-$(URT_VERSION) $(LDFLAGS)
TARGET := urt_names

.PHONY: all dep clean
all: dep $(TARGET)
	@$(SHELL_NOOP)
clean:
	-$(RM) $(TARGET) *.o Makefile.dep
dep: Makefile.dep
	@$(SHELL_NOOP)
LIB_HDR :=
include $(BASE)/include/Makefile.src
include $(SYS)/include/Makefile.src
VPATH := $(BASE)/include:$(SYS)/include
Makefile.dep: main.c $(LIB_HDR)
	$(CC) $(CFLAGS) $(CC_M) $(filter-out %.h,$^) > Makefile.dep

$(TARGET): main.o $(BASE)/build/shared/liburt-$(URT_VERSION).so
	$(LD) $(LD_o) $@ $(filter-out %.so,$^) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<

-include Makefile.dep
