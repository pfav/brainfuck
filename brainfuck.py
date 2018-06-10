import os
import sys

def usage():
    print "%s script.b" % sys.argv[0]
    sys.exit(127)





class Machine:
    def __init__(self, filename):
        self.filename = filename
        self.code = []
        self.codePos = 0
        self.tape = []
        self.tapePos = 0

        try:
            with open(filename, 'r') as fp:
                self.code = [ ord(ch) for ch in fp.read() ]
        except Exception as err:
            print err
            sys.exit(127)

    def run(self, skip = False):
        while self.tapePos >= 0 and self.codePos < len(self.code):
            if self.tapePos >= len(self.tape):
                self.tape.append(0)

            if self.code[self.codePos] == ord('['):
                self.codePos += 1
                oldPos = self.codePos
                while self.run(self.tape[self.tapePos] == 0):
                    self.codePos = oldPos
            elif self.code[self.codePos] == ord(']'):
                return self.tape[self.tapePos] != 0
            elif not skip:
                ch = self.code[self.codePos]
                if ch == ord('+'):
                    if self.tape[self.tapePos] < 255:
                        self.tape[self.tapePos] += 1
                elif ch == ord('-'):
                    if self.tape[self.tapePos] < 255:
                        self.tape[self.tapePos] -= 1
                elif ch == ord('>'): self.tapePos += 1
                elif ch == ord('<'): self.tapePos -= 1
                elif ch == ord('.'): os.write(2, chr(self.tape[self.tapePos]))
                elif ch == ord(','): self.tape[self.tapePos] = ord(os.read(0, 1))

            self.codePos += 1
        return True


if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) < 1:
        usage()

    filename = args[0]
    m = Machine(filename)
    m.run()
