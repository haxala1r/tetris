#!/bin/sh

OBJS=""

for f in $(find . -name "*.c")
do
	gcc -c "${f}" -o "${f%.c}.o"
	OBJS="${OBJS} ${f%.c}.o"
done

gcc $OBJS -lSDL2 -lSDL2_ttf -pthread -o game.elf
