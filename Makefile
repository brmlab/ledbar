CFLAGS=$(shell sdl-config --cflags) -Wall
LDFLAGS=$(shell sdl-config --libs)

all: ledbar

ledbar: ledbar.c
	gcc ledbar.c -o ledbar $(CFLAGS) $(LDFLAGS)

clean:
	rm -f ledbar
