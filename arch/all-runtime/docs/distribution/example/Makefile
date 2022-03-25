SRC = Calculator.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

CFLAGS = -g -O0 -I/usr/include/axrt-4.0 -specs=/usr/lib/x86_64-linux-gnu/axrt-4.0/axrt.specs
USER_LIBS =
LDFLAGS = -shared -L/usr/lib/x86_64-linux-gnu/axrt-4.0 -specs=/usr/lib/x86_64-linux-gnu/axrt-4.0/axrt.specs

# axrt.ld needs to be added to list of objects passed to linker for proper section ordering
# axrt.specs needs to be used for default libraries selection

Calculator: $(OBJ) /usr/lib/x86_64-linux-gnu/axrt-4.0/axrt.ld
	$(CC) -o $@ $^ $(LDFLAGS) $(USER_LIBS)

-include $(DEP)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) Calculator
