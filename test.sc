array x[5][10];
counter = 0;

def f2(x[]) {
    puti(x[2]);
    x[2] = 3;
}

def f1(x[]) {
    f2(x);
}

array y[5];
y[2] = 1;
f1(y);