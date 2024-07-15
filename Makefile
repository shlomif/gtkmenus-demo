#
# Makefile
# Shlomi Fish, 2024-07-15 10:18
#

all: main

main: gtkmenus.c
	gcc -o $@ `pkg-config --cflags gtk+-3.0` gtkmenus.c `pkg-config --libs gtk+-3.0`


# vim:ft=make
#
