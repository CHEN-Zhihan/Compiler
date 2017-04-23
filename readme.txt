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

2. support of function overload, example:

def a(x) {
    puti(x);
}

def a(x, y) {
    for (i = 0; i != y; i = i + 1;) {
        puti(x);
    }
}

a(5) // call the first function.
a(5, 10) // call the second function.

3. Result of addition and subtraction between char and int will be cast into char.
Comparison between string is not allowed. Comparison between different types is not allowed.

4. Scope is used, example:
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