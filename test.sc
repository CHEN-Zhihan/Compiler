array x[2][5];
for (i = 0; i != 2; ++i;) {
    for (j = 0; j != 5; ++j;) {
        x[i][j] = geti();
    }
}

for (i = 0; i != 2; ++i;) {
    for (j = 0; j != 5; ++j;) {
        puti_(x[i][j]);
        puts_("  ");
    }
    puts("");
}