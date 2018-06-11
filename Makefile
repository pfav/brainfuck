CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic
CXX = g++
CXXFLAGS = -std=c++14 -Wall -Werror -pedantic
LDFLAGS =

all: brainfuck brainfuck_c

brainfuck : brainfuck.cc
	$(CXX) $(CXXFLAGS) -static -o $@ $< $(LDFLAGS)

brainfuck_c : brainfuck.c
	$(CC) $(CFLAGS) -static -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf brainfuck brainfuck_c
