SRC = Calculator.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

STARTUP_OBJ = /usr/lib/x86_64-linux-gnu/startup.o

CFLAGS = -D__AROS__ -g -O0 -I/usr/include/axrt
USER_LIBS = -lmuimaster -lintuition -ldos -llocale -lutility -lmui -lexec
LDFLAGS = -specs=/usr/lib/x86_64-linux-gnu/axrt.specs

# axrt.ld needs to be added to list of objects passed to linker for proper section ordering
# startup.o needs to be added to list of objects to pull in initialization code
# axrt.specs needs to be used for default libraries selection

Calculator: $(OBJ) $(STARTUP_OBJ) /usr/lib/x86_64-linux-gnu/axrt.ld
	$(CC) -o $@ $^ $(LDFLAGS) $(USER_LIBS)

-include $(DEP)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) Calculator
