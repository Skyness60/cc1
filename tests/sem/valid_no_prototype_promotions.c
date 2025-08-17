int puts(); /* no-prototype declaration */

void g(float x, short s, char c) {
    /* default promotions apply for all args due to no-prototype puts */
    puts();
}
