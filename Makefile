#
# Makefile
# Shlomi Fish, 2024-07-15 10:18
#

all: main

CFLAGS = -g -Wall -Wextra

main: gtkmenus.c demo.c
	gcc $(CFLAGS) -o $@ `pkg-config --cflags gtk+-3.0` demo.c gtkmenus.c `pkg-config --libs gtk+-3.0`

demo.c: demo.gresource.xml
	glib-compile-resources --generate-header $<
	glib-compile-resources --generate-source $<

clean:
	rm -f main demo.c

# vim:ft=make
#
