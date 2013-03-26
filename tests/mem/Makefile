include ../../Makefile.config
include ../../Makefile.common

BASE := $(abspath $(CURDIR)/../..)
include $(BASE)/build/Makefile.build
LDFLAGS := -L$(BASE)/build/shared -lurt-$(URT_VERSION) $(LDFLAGS)
TARGET := test
TARGET2 := add

.PHONY: all dep clean
all: dep $(TARGET) $(TARGET2)
	@$(SHELL_NOOP)
clean:
	-$(RM) $(TARGET) $(TARGET2) *.o Makefile.dep
dep: Makefile.dep
	@$(SHELL_NOOP)
LIB_HDR :=
include $(BASE)/include/Makefile.src
include $(SYS)/include/Makefile.src
VPATH := $(BASE)/include:$(SYS)/include
Makefile.dep: main.c add.c $(LIB_HDR)
	$(CC) $(CFLAGS) $(CC_M) $(filter-out %.h,$^) > Makefile.dep

$(TARGET): main.o $(BASE)/build/shared/liburt-$(URT_VERSION).so
	$(LD) $(LD_o) $@ $(filter-out %.so,$^) $(LDFLAGS)
$(TARGET2): add.o $(BASE)/build/shared/liburt-$(URT_VERSION).so
	$(LD) $(LD_o) $@ $(filter-out %.so,$^) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(CC_o) $@ $<

.PHONY: execute_test valgrind_check
execute_test:
	@./$(TARGET) > /dev/null 2>&1
valgrind_check:
	@valgrind --leak-check=full ./$(TARGET) | grep --color=never 'in use at exit:\|== Invalid'

-include Makefile.dep
