array x[5][10];
x[2][10] = geti();
puti(x[2][3+7]);

def f2(x[]) {
    array b[3];
    puti(x[1]);
}

def f1() {
    array x[2];
    f2(x);
}

f1();