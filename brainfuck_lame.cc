#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using std::cerr;
using std::cin;
using std::endl;
using std::vector;
using std::ifstream;
using std::numeric_limits;

using pos_t = long long;
using cell_t = unsigned char;

struct Code {
public:
  using isiterator = std::istreambuf_iterator<char>;
  Code(ifstream &input) : code(isiterator(input), isiterator()), Pos{0} {}

  bool done() { return Pos >= static_cast<pos_t>(code.size()); }

  vector<cell_t> code{};
  pos_t Pos{};
};

struct Tape : public vector<cell_t> {
public:
  using vector<cell_t>::vector;

  pos_t Pos{};
};

class Interpreter {
public:
  Interpreter(ifstream &input) : code{input}, tape{} {}

  bool run(bool skip = false) {

    while (tape.Pos >= 0 && !code.done()) {
      if (tape.Pos >= static_cast<pos_t>(tape.size())) {
        tape.push_back(0);
      }

      if (code.code[code.Pos] == '[') {
        code.Pos++;
        const pos_t oldPos = code.Pos;
        while (run(tape[tape.Pos] == 0)) {
          code.Pos = oldPos;
        }
      } else if (code.code[code.Pos] == ']') {
        return tape[tape.Pos] != 0;
      } else if (!skip) {
        switch (code.code[code.Pos]) {
        case '+': {
          if (tape[tape.Pos] <
              numeric_limits<decltype(tape)::value_type>::max())
            tape[tape.Pos]++;
        } break;
        case '-': {
          if (tape[tape.Pos] > 0)
            tape[tape.Pos]--;
        } break;
        case '>': {
          tape.Pos++;
        } break;
        case '<': {
          tape.Pos--;
        } break;
        case '.': {
          cerr << tape[tape.Pos];
        } break;
        case ',': {
          cin >> tape[tape.Pos];
        } break;
        }
      }

      code.Pos++;
    }

    return true;
  }

private:
  Code code;
  Tape tape;
};

int main(int c, char **v) {
  if (c < 2) {
    cerr << "usage: brainfuck filename.b" << endl;
    return 127;
  }

  const std::string filename{v[1]};

  struct stat st {};
  if (stat(filename.c_str(), &st) == -1) {
    cerr << "brainfuck: " << strerror(errno) << endl;
    return 127;
  }

  if (!S_ISREG(st.st_mode)) {
    cerr << "brainfuck: " << filename << " not a regular file" << endl;
    return 127;
  }

  ifstream ff(filename);
  if (ff) {
    Interpreter interpreter{ff};
    interpreter.run();
  } else {
    cerr << "brainfuck: " << strerror(errno) << endl;
    return 127;
  }
}
