CC = gcc
CFLAGS = -g -O0 -I/usr/include/libpng16  -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16   -DLOCALEDIR=\"/usr/local/share/locale\"
CPPFLAGS =
LDFLAGS =  -lX11 -lXft -lXrender -lXext -lXmu -lXinerama
VPATH=.:os

OBJECTS = main.o error.o screen.o cursor.o place.o settings.o misc.o border.o grab.o binding.o font.o color.o \
            client.o timing.o hint.o desktop.o event.o move.o resize.o status.o outline.o clientlist.o debug.o \
            group.o match.o gradient.o

EXE = jwm

.SUFFIXES: .o .h .c

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) -o $(EXE) $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

$(OBJECTS): *.h config.h

clean:
	rm -f $(OBJECTS) $(EXE) core

