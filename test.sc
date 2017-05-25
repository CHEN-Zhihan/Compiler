array a[5];

for (i = 0; i != 5; ++i;) {
    a[i] = geti();
}

for (i = 0 ; i != 5 ; ++i;) {
    max = a[i];
    index = i;
    for (j = i + 1; j < 5 ; ++ j;) {
        if (a[j] > max) {
            max = a[j];
            index = j;
        }
    }
    temp = a[i];
    a[i] = max;
    a[index] = temp;
}

for (i = 0; i != 5; ++i;) {
    puti(a[i]);
}