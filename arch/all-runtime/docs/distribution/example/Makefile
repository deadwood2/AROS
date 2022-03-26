SRC = Calculator.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

CFLAGS = -g -O0 -I/usr/include/axrt-4.0 -specs=/usr/lib/x86_64-linux-gnu/axrt-4.0/axrt.specs
LDFLAGS = -L/usr/lib/x86_64-linux-gnu/axrt-4.0 -specs=/usr/lib/x86_64-linux-gnu/axrt-4.0/axrt.specs
USER_LIBS =

# Flags needed for dual-run capability
CFLAGS += -fPIC
LDFLAGS += -shared

# axrt.ld needs to be added to list of objects passed to linker for proper section ordering

Calculator: $(OBJ) /usr/lib/x86_64-linux-gnu/axrt-4.0/axrt.ld
	$(CC) -o $@ $^ $(LDFLAGS) $(USER_LIBS)
	patchelf-debug --add-debug $@

-include $(DEP)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(OBJ) $(DEP) Calculator
