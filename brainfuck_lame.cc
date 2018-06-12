#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

using namespace std;

using pos_t = long long;

struct Machine{
  string code{};
  pos_t codePos{};
  string tape{};
  pos_t tapePos{};
  
  bool run(bool skip = false){
    while(tapePos >= 0 && codePos < static_cast<pos_t>(code.length())){
      if (tapePos >= static_cast<pos_t>(tape.length())){
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
        case '-': tape[tapePos] == 0 ? 0 : tape[tapePos]--; break;
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



int main(int c, char **v){
  if (c < 2){
    cerr << "usage: brainfuck filename.b" << endl;
    return 127;
  }

  struct stat st{};
  if (stat(v[1], &st) == -1){
    cerr << "brainfuck: " << strerror(errno) << endl;
    return 127;
  }

  if (!S_ISREG(st.st_mode)){
    cerr << "brainfuck: " << v[1] << " not a regular file" << endl;
    return 127;
  }


  ifstream ff(v[1]);
  if (ff){
    Machine m{};
    stringstream ss;

    ss << ff.rdbuf();
    m.code = ss.str();

    m.run();
  } else {
    cerr << "brainfuck: " << strerror(errno) << endl;
    return 127;
  }
}



