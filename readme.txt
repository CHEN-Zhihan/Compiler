Name: CHEN Zhihan, UID: 3035142261
Compilation: make

Execution: ./c6 target > outfile
            ./cvm outfile

Other information:
1. all function definition starts with def.

i.e., the factorial function shall be:

def factorial(n) {
    if (n == 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

2. Scope and reference check are adopted, example:
a = 1;
c = 5;
if (a == 1) {
    b = 2;
    c = a + b;
    puti(b); // prints 2
    puti(c); // prints 3
} else {
    b = 3;
    c = a + b;
    puti(b); // prints 3
    puti(c); // prints 4
}
puti(b); //illegal
puti(c); // prints 3