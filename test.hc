def main(main, a) {
    if (main == 5) {
        return 4;
    }
    return main(main - 1, a);
}

puti(main(6, 4));