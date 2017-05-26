array x[10];

for (i = 0; i != 10; ++i;) {
    x[i] = 1000-i;
}

for (i = 0; i != 10; ++i;) {
    min = i;
    for (j = i; j != 10; ++j;) {
        if (x[j] < x[min]) {
            min = j;
        }
    }
    temp = x[min];
    x[min] = x[i];
    x[i] = temp;
}

for (i = 0; i != 10; ++i;) {
    puti(x[i]);
}