def addition(a, b) {
    return a + b;
}


def main(a, b) {
    a = 20;
    @a = addition(@a, a);
    return @a;
}

a = 5;

puti(main(a, 5));
puti(a);