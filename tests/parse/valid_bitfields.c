struct S {
    unsigned a:3;
    int b:5;
};

struct U {
    int :4; /* unnamed bit-field */
    int c:1;
};
