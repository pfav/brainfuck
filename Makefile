CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic -O3 -march=native -fomit-frame-pointer
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror -O3 -pedantic -march=native -fomit-frame-pointer
LDFLAGS =

LOADER = /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2

all: brainfuck brainfuck_c brainfuck_asm

brainfuck : brainfuck.cc
	cc_args $(CXX) $(CXXFLAGS) -static -o $@ $< $(LDFLAGS)

brainfuck_lame : brainfuck_lame.cc
	cc_args $(CXX) $(CXXFLAGS) -static -o $@ $< $(LDFLAGS)

brainfuck_c : brainfuck.c
	$(CC) $(CFLAGS) -static -o $@ $< $(LDFLAGS)

brainfuck_asm : brainfuck_asm.tmp
	as --64 --gstabs -o $@.o $<
	ld -dynamic-linker $(LOADER) -o $@ -lc $@.o

brainfuck_asm.tmp : brainfuck.s
	cpp -I /usr/include/asm -o $@ $<

.PHONY: clean
clean:
	rm -rf brainfuck brainfuck_c brainfuck_asm *.o *.tmp
