SRC = Calculator.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

RUNTIME_SDK = ..
STARTUP_OBJ = $(RUNTIME_SDK)/lib/startup.o

CFLAGS = -D__AROS__ -g -O0 -I$(RUNTIME_SDK)/include
LIBS = -lmuimaster -lintuition -ldos -llocale -lutility -lmui -lexec -lruntimestartup -llibinit -ldl -lautoinit -lamiga
LDFLAGS = -L$(RUNTIME_SDK)/lib

# runtime.ld needs to be added to list of objects passed to linker for proper section ordering
# startup.o needs to be added to list of objects to pull in initialization code

Calculator: $(OBJ) $(STARTUP_OBJ) $(RUNTIME_SDK)/lib/runtime.ld
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

-include $(DEP)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) Calculator
