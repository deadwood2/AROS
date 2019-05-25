SRC = Calculator.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

RUNTIME_SDK = ..
STARTUP_OBJ = $(RUNTIME_SDK)/lib/startup.o

CFLAGS = -D__AROS__ -g -O0 -I$(RUNTIME_SDK)/include
RUNTIME_LIBS = -lstdc -llibinit -lautoinit -lamiga -laxrt
USER_LIBS = -lmuimaster -lintuition -ldos -llocale -lutility -lmui -lexec
LDFLAGS = -L$(RUNTIME_SDK)/lib -Wl,-rpath,'$$ORIGIN'

# runtime.ld needs to be added to list of objects passed to linker for proper section ordering
# startup.o needs to be added to list of objects to pull in initialization code

Calculator: $(OBJ) $(STARTUP_OBJ) $(RUNTIME_SDK)/lib/runtime.ld
	$(CC) -o $@ $^ $(LDFLAGS) $(USER_LIBS) $(RUNTIME_LIBS)

-include $(DEP)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) Calculator
