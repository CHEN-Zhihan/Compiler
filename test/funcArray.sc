array x[5][10];
counter = 0;

def f2(x[]) {
    puti(x[2]);
    x[2] = 3;
}

def f1(x[]) {
    puti(x[2]);
    x[2] = 2;
    puti(@y[2]);
    f2(x);
    x[2] = 4;
    f2(@y);
    puti(@y[2]);
    @y[2] = 5;
}

array y[5];
y[2] = 1;
f1(y);
puti(y[2]);