#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wunused-result"

#define MINSIZE (1024)
typedef long long pos_t;
static struct {
  char *code;
  pos_t codePos;
  size_t codeSize;
  char *tape;
  pos_t tapePos;
  size_t tapeSize;
} machine = {0};

void machine_free() {
  if (machine.code) {
    free(machine.code);
    machine.code = 0;
  }
  if (machine.tape) {
    free(machine.tape);
    machine.tape = 0;
  }
}

__attribute__((noreturn)) void err_quit(int n, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(n);
}

bool run(bool skip) {

  while (machine.tapePos >= 0 && machine.codePos < (machine.codeSize - 1)) {
    if (machine.tapePos >= machine.tapeSize) {
      size_t oldPos = machine.tapeSize;
      machine.tapeSize *= 2;
      machine.tape = realloc(machine.tape, machine.tapeSize);
      memset(machine.tape + oldPos, 0x0, machine.tapeSize - oldPos);
    }

    if (machine.code[machine.codePos] == '[') {
      machine.codePos++;
      pos_t oldPos = machine.codePos;
      while (run(machine.tape[machine.tapePos] == 0)) {
        machine.codePos = oldPos;
      }
    } else if (machine.code[machine.codePos] == ']') {
      return machine.tape[machine.tapePos] != 0;
    } else if (!skip) {
      switch (machine.code[machine.codePos]) {
      case '+': {
        machine.tape[machine.tapePos]++;
      } break;
      case '-': {
        machine.tape[machine.tapePos]--;
      } break;
      case '>': {
        machine.tapePos++;
      } break;
      case '<': {
        machine.tapePos--;
      } break;
      case '.': {
        write(2, &machine.tape[machine.tapePos], 1);
      } break;
      case ',': {
        read(0, &machine.tape[machine.tapePos], 1);
      } break;
      }
    }
    machine.codePos++;
  }
  return true;
}

int main(int c, char **v) {
  if (c < 2) {
    err_quit(127, "usage: %s brainfuck.b\n", v[0]);
  }

  const char *filename = v[1];

  struct stat st;
  if (stat(filename, &st) == -1) {
    err_quit(127, "brainfuck: %s\n", strerror(errno));
  } else if (!S_ISREG(st.st_mode)) {
    err_quit(127, "brainfuck: %s not a regular file\n", v[1]);
  }

  atexit(machine_free);
  machine.codeSize = st.st_size + 1;

  machine.code = (char *)malloc(machine.codeSize);
  if (!machine.code) {
    err_quit(127, "brainfuck: failed to allocate memory\n");
  }
  memset(machine.code, 0x0, machine.codeSize);

  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    err_quit(127, "brainfuck: unable to open %s\n", filename);
  }
  read(fd, machine.code, machine.codeSize - 1);

  machine.tapeSize = machine.codeSize * 2;
  machine.tape = (char *)malloc(machine.tapeSize);
  if (!machine.tape) {
    err_quit(127, "brainfuck: failed to allocate memory\n");
  }
  memset(machine.tape, 0x0, machine.tapeSize);

  run(false);

  return EXIT_SUCCESS;
}
