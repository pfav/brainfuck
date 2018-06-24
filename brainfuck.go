package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"os"
	"unicode"
)

func init() {
	flag.Parse()
}

type Code struct {
	data []byte
	pos  int
}

func (c Code) Done() bool {
	return c.pos >= len(c.data)
}
func (c Code) Curr() byte {
	return c.data[c.pos]
}

var code Code

type Tape struct {
	data []byte
	pos  int
}

func (t Tape) Curr() byte {
	return t.data[t.pos]
}

func (t Tape) Push(b byte) {
	t.data = append(t.data, b)
}

func (t *Tape) Incr() {
	t.data[t.pos]++
}
func (t *Tape) Descr() {
	t.data[t.pos]--
}

var tape Tape

func usage() {
	fmt.Printf("usage: %s brainfuck.b\n", os.Args[0])
	os.Exit(127)
}

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	args := flag.Args()

	filename := args[0]

	fd, err := os.Open(filename)
	if err != nil {
		fmt.Fprintf(os.Stderr, "brainfuck: %s\n", err)
		os.Exit(127)
	}
	defer fd.Close()

	var buf bytes.Buffer
	io.Copy(&buf, fd)

	b := buf.Bytes()
	code = Code{data: b, pos: 0}
	tape = Tape{data: make([]byte, len(b)), pos: 0}

	run(false)
}

func run(skip bool) bool {
	for tape.pos >= 0 && !code.Done() {
		if tape.pos >= len(tape.data) {
			tape.Push(0)
		}

		if code.Curr() == '[' {
			code.pos++
			oldPos := code.pos
			for run(tape.Curr() == 0) {
				code.pos = oldPos
			}
		} else if code.Curr() == ']' {
			return tape.Curr() != 0
		} else if !skip {
			switch code.Curr() {
			case '+':
				if tape.Curr() < 255 {
					tape.Incr()
				}
			case '-':
				if tape.Curr() > 0 {
					tape.Descr()
				}
			case '>':
				tape.pos++
			case '<':
				tape.pos--
			case '.':
				fmt.Printf("%c", tape.Curr())
			case ',':
				b := make([]byte, 1)
				os.Stdin.Read(b)
				if unicode.IsPrint(rune(b[0])) {
					tape.data[tape.pos] = b[0]
				} else {
					fmt.Println()
				}
			}
		}

		code.pos++
	}

	return true
}
