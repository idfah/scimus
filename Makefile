SHELL = /bin/bash
CC    = gcc

GLLIBS  = -lGL -lGLU -lglut -lm
PNGLIBS = `libpng-config --cflags --libs`

LDFLAGS  = $(GLLIBS) $(PNGLIBS)
CPPFLAGS = 
CFLAGS   = -Wall -O2

MODS = pngLoader.o navigator.o doubleHelix.o primatives.o

all:  scimus

mods: $(MODS)

scimus:  scimus.c scimus.h $(MODS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o scimus scimus.c $(MODS) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(MODS)

remove: clean
	rm -f scimus
