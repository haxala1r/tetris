#!/bin/sh
# Clean every file ending in ".o"
# game.elf isn't deleted
for F in $(find ./ -name "*.o")
do
	rm "${F}"
done
