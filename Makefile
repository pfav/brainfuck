CC = g++
CFLAGS = -std=c++14 -Wall -Werror -pedantic
LDFLAGS =

all: brainfuck

brainfuck : brainfuck.cc
	$(CC) $(CFLAGS) -static -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf brainfuck
