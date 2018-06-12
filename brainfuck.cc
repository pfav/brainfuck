#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <vector>
#include <memory>
#include <utility>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>


using std::cerr;
using std::cin;
using std::endl;
using std::vector;
using std::ifstream;
using std::numeric_limits;

using pos_t = unsigned long long;
using cell_t = unsigned char;



class Code {
public:

  using isiterator = std::istreambuf_iterator<char>;
  Code(ifstream& input)
    : code_(isiterator(input), isiterator()),
      pos_{0}
  {
  }

  bool done(){ return pos_ >= code_.size(); }

  cell_t& curr() {  return code_.at(pos_); }
  const cell_t& curr() const {  return code_.at(pos_); }

  pos_t& pos(){ return pos_; }
  const pos_t pos() const { return pos_; }
private:
  vector<cell_t> code_{};
  pos_t pos_{};
};

class Tape : public vector<cell_t>
{
public:
  using vector<cell_t>::vector;

  pos_t& pos() { return pos_; }
  const pos_t& pos() const { return pos_; }

  cell_t& curr() { return vector::at(pos_); }
  const cell_t& curr() const { return vector::at(pos_); }
private:
  pos_t pos_{};
};


class Interpreter
{
public:
  Interpreter(ifstream& input)
    : code{input},
      tape{}
  {}

  bool run(bool skip = false)
  {

    while(tape.pos() >= 0 && !code.done()){
      if (tape.pos() >= tape.size()){
        tape.push_back(0);
      }
      //cerr << code.curr() << ' ' << std::boolalpha << (tape.curr() == 0) << std::endl;

      if (code.curr() == '[') {
        code.pos()++;
        const pos_t oldPos = code.pos();
        while(run(tape.curr() == 0)){
          code.pos() = oldPos;
        }
      } else if (code.curr() == ']'){
        return tape.curr() != 0;
      } else if (!skip){
        switch (code.curr()){
        case '+': { if (tape.curr() < numeric_limits<decltype(tape)::value_type>::max()) tape.curr()++; } break;
        case '-': { if (tape.curr() > 0) tape.curr()--; } break;
        case '>': { tape.pos()++; } break;
        case '<': { tape.pos()--; } break;
        case '.': { cerr << tape.curr(); } break;
        case ',': { cin >> tape.curr(); } break;
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


/*


struct Machine{
  string code{};
  pos_t codePos{};
  string tape{};
  pos_t tapePos{};
  
  bool run(bool skip = false){
    while(tapePos >= 0 && codePos < code.length()){
      if (tapePos >= tape.length()){
        tape += '\0';
      }

      if (code[codePos] == '['){
        codePos++;
        int oldPos = codePos;
        while(run(tape[tapePos] == '\0'))
          codePos = oldPos;
      } else if (code[codePos] == ']'){
        return tape[tapePos] != '\0';
      } else if (!skip){
        switch(code[codePos]){
        case '+': tape[tapePos] == numeric_limits<string::value_type>::max() ? 0 : tape[tapePos]++; break;
        case '-': tape[tapePos] == numeric_limits<string::value_type>::max() ? 0 : tape[tapePos]--; break;
        case '>': tapePos++; break;
        case '<': tapePos--; break;
        case '.': cerr << tape[tapePos]; break;
        case ',': cin >> tape[tapePos]; break;
        }
      }

      codePos++;
    }
    return true;
  }

};

*/

int main(int c, char **v){
  if (c < 2){
    cerr << "usage: brainfuck filename.b" << endl;
    return 127;
  }

  const std::string filename{v[1]};

  struct stat st{};
  if (stat(filename.c_str(), &st) == -1){
    cerr << "brainfuck: " << strerror(errno) << endl;
    return 127;
  }

  if (!S_ISREG(st.st_mode)){
    cerr << "brainfuck: " << filename << " not a regular file" << endl;
    return 127;
  }

  ifstream ff(filename);
  if (ff){
    Interpreter interpreter{ff};
    interpreter.run();
  } else {
    cerr << "brainfuck: " << strerror(errno) << endl;
    return 127;
  }
}



