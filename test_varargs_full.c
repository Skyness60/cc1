/* Test complet varargs avec différents types */
#include <stdio.h>

int main() {
    int i = 42;
    char c = 'A';
    short s = 1000;
    long l = 123456L;
    float f = 3.14f;
    double d = 2.718;
    char *str = "test";
    
    /* Test promotions dans varargs */
    printf("int: %d\n", i);
    printf("char: %c (promoted to int: %d)\n", c, c);
    printf("short: %d\n", s); 
    printf("long: %ld\n", l);
    printf("float: %f (promoted to double)\n", f);
    printf("double: %f\n", d);
    printf("string: %s\n", str);
    
    /* Test multiple args */
    printf("Multiple: %d %c %s %f\n", i, c, str, f);
    
    return 0;
}
