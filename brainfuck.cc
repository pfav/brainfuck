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

class Code {
public:
  using value_type = cell_t;
  using isiterator = std::istreambuf_iterator<char>;

  Code(ifstream &input) : code_(isiterator(input), isiterator()), pos_{0} {}

  bool done() { return pos_ >= static_cast<pos_t>(code_.size()); }

  cell_t &curr() { return code_.at(pos_); }
  const cell_t &curr() const { return code_.at(pos_); }

  pos_t &pos() { return pos_; }
  const pos_t &pos() const { return pos_; }

private:
  vector<cell_t> code_{};
  pos_t pos_{};
};

class Tape {
public:
  using value_type = cell_t;
  pos_t &pos() { return pos_; }
  const pos_t &pos() const { return pos_; }

  cell_t &curr() { return tape_.at(pos_); }
  const cell_t &curr() const { return tape_.at(pos_); }

  size_t size() const { return tape_.size(); }
  void push(cell_t item){ tape_.push_back(item); }
private:
  vector<cell_t> tape_{};
  pos_t pos_{};
};

class Interpreter {
public:
  Interpreter(ifstream &input) : code{input}, tape{} {}

  bool run(bool skip = false) {

    while (tape.pos() >= 0 && !code.done()) {
      if (tape.pos() >= static_cast<pos_t>(tape.size())) {
        tape.push(0);
      }

      if (code.curr() == '[') {
        code.pos()++;
        const pos_t oldPos = code.pos();
        while (run(tape.curr() == 0)) {
          code.pos() = oldPos;
        }
      } else if (code.curr() == ']') {
        return tape.curr() != 0;
      } else if (!skip) {
        switch (code.curr()) {
        case '+': {
          if (tape.curr() < numeric_limits<decltype(tape)::value_type>::max())
            tape.curr()++;
        } break;
        case '-': {
          if (tape.curr() > 0)
            tape.curr()--;
        } break;
        case '>': {
          tape.pos()++;
        } break;
        case '<': {
          tape.pos()--;
        } break;
        case '.': {
          cerr << tape.curr();
        } break;
        case ',': {
          cin >> tape.curr();
        } break;
        }
      }

      code.pos()++;
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
