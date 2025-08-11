/* invalid: unknown punctuator / non-ASCII character */
int main(void) {
    return 0; /* euro sign below should trigger a lex error in strict mode */
    €
}
