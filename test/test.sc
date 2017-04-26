def x(a) {
    puti(a);
    return a + 1;
}

def y(b, a) {
    z = x(b + a);
    return z;
}

puti(y(10, 30)); 