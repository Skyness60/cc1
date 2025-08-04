/* Test exhaustif C89 pur - Tous les tokens possibles */
#include <stdio.h>
#include <stdlib.h>

/* Types de base et modifiers C89 */
char c = 'X';
signed char sc = '\n';
unsigned char uc = '\t';
short s = -32768;
unsigned short us = 65535U;
int i = 42;
signed int si = -2147483648L;
unsigned int ui = 4294967295UL;
long l = 2147483647L;
unsigned long ul = 4294967295UL;
float f = 3.14159f;
double d = 2.71828;
long double ld = 1.41421356L;

/* Nombres dans toutes les bases */
int decimal = 123;
int octal = 0777;
int hex_lower = 0xabcdef;
int hex_upper = 0XABCDEF;
float sci_notation = 1.23e-45f;
double exp_notation = 6.022E23;

/* Opérateurs arithmétiques */
void arithmetic_ops() {
    int a = 10, b = 3, result;
    result = a + b;
    result = a - b;
    result = a * b;
    result = a / b;
    result = a % b;
    
    /* Assignations composées */
    a += b;
    a -= b;
    a *= b;
    a /= b;
    a %= b;
    
    /* Incrémentation/décrémentation */
    ++a; a++; --b; b--;
}

/* Opérateurs de comparaison */
void comparison_ops() {
    int x = 5, y = 10;
    int cmp;
    cmp = (x == y);
    cmp = (x != y);
    cmp = (x < y);
    cmp = (x > y);
    cmp = (x <= y);
    cmp = (x >= y);
}

/* Opérateurs logiques */
void logical_ops() {
    int a = 1, b = 0, result;
    result = a && b;
    result = a || b;
    result = !a;
}

/* Opérateurs bitwise */
void bitwise_ops() {
    unsigned int x = 0xFF, y = 0xF0, result;
    result = x & y;
    result = x | y;
    result = x ^ y;
    result = ~x;
    result = x << 2;
    result = x >> 2;
    
    /* Assignations bitwise */
    x &= y;
    x |= y;
    x ^= y;
    x <<= 2;
    x >>= 2;
}

/* Pointeurs et adresses */
void pointer_ops() {
    int value = 42;
    int *ptr = &value;
    int deref = *ptr;
    
    int array[10];
    ptr = array;
    deref = ptr[0];
}

/* Structures et unions */
struct Point {
    int x, y;
};

union Data {
    int i;
    float f;
    char str[20];
};

typedef struct {
    char name[50];
    int age;
} Person;

/* Énumérations */
enum Color {
    RED,
    GREEN = 5,
    BLUE
};

/* Structures de contrôle */
void control_structures() {
    int i, j;
    
    if (i > 0) {
        /* vide */
    } else if (i < 0) {
        /* vide */
    } else {
        /* vide */
    }
    
    /* Opérateur ternaire */
    int max = (i > j) ? i : j;
    
    /* Switch */
    switch (i) {
        case 1:
            break;
        case 2:
        case 3:
            continue;
        default:
            goto end;
    }
    
    /* Boucles */
    while (i > 0) {
        i--;
    }
    
    do {
        i++;
    } while (i < 10);
    
    for (j = 0; j < 10; j++) {
        if (j == 5) break;
        if (j == 3) continue;
    }
    
    end:
        return;
}

/* Classes de stockage */
static int static_var = 100;
extern int extern_var;
register int fast_var;
auto int local_var;

/* Qualifieurs */
const int constant = 42;
volatile int hardware_reg;

/* Accès aux membres de structures */
void struct_access() {
    struct Point p;
    p.x = 10;
    p.y = 20;
    
    struct Point *ptr = &p;
    ptr->x = 30;
    ptr->y = 40;
    
    /* Taille d'un type */
    int size = sizeof(int);
    size = sizeof(struct Point);
}

/* Caractères d'échappement C89 */
void escape_chars() {
    char bell = '\a';
    char backspace = '\b';
    char formfeed = '\f';
    char newline = '\n';
    char carriage = '\r';
    char tab = '\t';
    char vtab = '\v';
    char backslash = '\\';
    char question = '\?';
    char quote = '\'';
    char dquote = '\"';
    char null_char = '\0';
    char octal_char = '\377';
    char hex_char = '\xFF';
}

/* Point d'entrée principal */
int main() {
    /* Déclarations avec initialisations */
    int numbers[] = {1, 2, 3, 4, 5};
    char string[] = "Hello, World!";
    
    struct Point origin = {0, 0};
    Person person;
    
    /* Test de tous les délimiteurs */
    int array[10];
    array[0] = 42;
    
    /* Appel de fonction */
    printf("Success: %d\n", 42);
    
    return 0;
}
