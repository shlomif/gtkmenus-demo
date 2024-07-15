#
# Makefile
# Shlomi Fish, 2024-07-15 10:18
#

all: main

main: gtkmenus.c demo.gresource
	gcc -o $@ `pkg-config --cflags gtk+-3.0` gtkmenus.c `pkg-config --libs gtk+-3.0`

demo.gresource: demo.gresource.xml
	glib-compile-resources --generate-source --generate-header $<

# vim:ft=make
#
