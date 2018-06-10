var fs = require("fs");
var args = process.argv.slice(2);


var code = {
    EOF: -1,
    str: '',
    pos: 0,
    getchar: function(){
        if (this.pos < this.str.length) return this.str[this.pos++]
        else return this.EOF
    }
};

var tape = {
    str: [],
    pos: 0,
};

var fd = -1; // open in main
function getch(){
    var buf = new Uint8Array(1);
    fs.readSync(fd, buf, 0, 1, null);
    return buf[0];
}

// brainfuck interpreter
function interpret(skip){
    var skip = skip || false;

    while (tape.pos >= 0 && code.pos < code.str.length){
        if (tape.pos >= tape.str.length){
            tape.str.push(0)
        }

        if (code.str[code.pos] == '['){
            code.pos++;
            var oldpos = code.pos;
            while (interpret(tape.str[tape.pos] == 0)){
                code.pos = oldpos;
            }
        } else if (code.str[code.pos] == ']'){
            return tape.str[tape.pos] != 0;
        } else if (!skip) {
            switch(code.str[code.pos]) {
            case '+': tape.str[tape.pos] > 255 ? 0 : tape.str[tape.pos]++; break;
            case '-': tape.str[tape.pos] > 255 ? 0 : tape.str[tape.pos]--; break;
            case '>': tape.pos++; break;
            case '<': tape.pos--; break;
            case '.': process.stdout.write(String.fromCharCode(tape.str[tape.pos])); break;
            case ',': tape.str[tape.pos] = getch(); break;
            }
        }
        code.pos++;
    }

    return true;
}

function usage(){
    console.log("usage: brainfuck.js filename.b")
    process.exit(127)
}

function errorFatal(msg){
    console.log("[ERROR]", msg);
    process.exit(127);
}

function main(args){
    if (args.length < 1){
        usage();
    }

    fd = fs.openSync('/dev/stdin', 'rs');

    var filename = args[0]
    if (! fs.existsSync(filename)){
        errorFatal("Unable to find file " + filename);
    }
    code.str = fs.readFileSync(filename, { encoding: 'utf8' });
    code.pos = 0;
    interpret();

    if (fd > -1)
        fs.closeSync(fd);
}
main(args);
