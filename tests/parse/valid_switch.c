int f(int x) {
    switch (x) {
    case 1:
        x++;
        break;
    case 2:
    case 3:
        x += 10;
        break;
    default:
        x = 0;
    }
    return x;
}
