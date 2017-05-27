def f(x[][5], size) {
    for (i = 0; i != size; ++i;) {
        puti(x[i][2]);
    }
}

array x[5][5] = 5;
array z[3][5] = 3;
f(x, 5);
f(z, 3);