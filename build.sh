#!/bin/sh

# This file doesn't do anything particularly complex/important.
# It just compiles all C files it finds
# in this repository and links them together.

# Also, you need to have GCC, SDL2 and SDL2_ttf installed
# to be able to build this.

OBJS=""

for f in $(find . -name "*.c")
do
	gcc -c "${f}" -o "${f%.c}.o"
	OBJS="${OBJS} ${f%.c}.o"
done

gcc $OBJS -lSDL2 -lSDL2_ttf -pthread -o game.elf
