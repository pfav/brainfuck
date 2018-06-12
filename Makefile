CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic -O3 -fomit-frame-pointer
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror -O3 -pedantic -fomit-frame-pointer
LDFLAGS =

all: brainfuck brainfuck_c

brainfuck : brainfuck.cc
	cc_args $(CXX) $(CXXFLAGS) -static -o $@ $< $(LDFLAGS)

brainfuck_c : brainfuck.c
	$(CC) $(CFLAGS) -static -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf brainfuck brainfuck_c
