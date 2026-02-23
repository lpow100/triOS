int pow(int value) {
    int out = 1;
    for (int i = 0; i < value; i++) out *= value;
    return out;
}