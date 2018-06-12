const fs = require("fs");
const args = process.argv.slice(2);

let code = {
    s: '',
    pos: 0,
    open: function(filename){
        this.s = fs.readFileSync(filename, { encoding: 'utf8' });
    },
    size: function(){ return this.s.length; },
    curr: function(){ return this.s[this.pos]; }
};

let tape = {
    t: [0],
    pos: 0,
    size: function(){ return this.t.length; },
    curr: function(){ return this.t[this.pos]; },
    set: function(ch){ this.t[this.pos] = ch; },
    incrValue: function(){ this.t[this.pos]++; },
    descValue: function(){ this.t[this.pos]--; },
    resize: function(){ this.t = this.t.concat(Array(this.size()).fill(0)); }
};

let io = {
    inFD : -1,
    outFD : -1,
    init: function(){
        this.inFD = fs.openSync('/dev/stdin', 'rs');
        this.outFD = fs.openSync('/dev/stdout', 'w');
    },
    get: function (){
        let buf = new Uint8Array(1);
        fs.readSync(this.inFD, buf, 0, 1, null);
        return buf[0];
    },
    put: function (ch){
        fs.writeSync(this.outFD, String.fromCharCode(ch));
    },
    close: function(){
        if (this.inFD > -1) fs.closeSync(this.inFD);
        if (this.outFD > -1) fs.closeSync(this.outFD);
    }
};

// brainfuck interpreter
function interpret(skip_){
    let skip = skip_ || false;

    while (tape.pos >= 0 && code.pos < code.size()){
        if (tape.pos >= tape.size()){
            tape.resize();
        }

        if (code.curr() == '['){
            code.pos++;
            let oldpos = code.pos;
            while (interpret(tape.curr() == 0)){
                code.pos = oldpos;
            }
        } else if (code.curr() == ']'){
            return tape.curr() != 0;
        } else if (!skip) {
            switch(code.curr()) {
            case '+': if (tape.curr() < 255) tape.incrValue(); break;
            case '-': if (tape.curr() < 255) tape.descValue();  break;
            case '>': tape.pos++; break;
            case '<': tape.pos--; break;
            case '.': io.put(tape.curr()); break;
            case ',': tape.set(io.get()); break;
            }
        }
        code.pos++;
    }

    return true;
}

function usage(){
    console.log("usage: brainfuck.js filename.b");
    process.exit(127);
}

function errorFatal(msg){
    console.log("[ERROR]", msg);
    process.exit(127);
}

function main(args){
    if (args.length < 1){
        usage();
    }

    let filename = args[0];
    if (! fs.existsSync(filename)){
        errorFatal("Unable to find file " + filename);
    }

    io.init();
    code.open(filename);
    interpret();
    io.close();
}

main(args);
