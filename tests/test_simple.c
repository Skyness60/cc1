/* Test C89 basique mais complet */
#include <stdio.h>

/* Tous les types de base C89 */
char c = 'A';
int i = 42;
short s = 100;
long l = 1000L;
unsigned u = 42U;
unsigned long ul = 1000UL;
float f = 3.14f;
double d = 2.71;

/* Toutes les bases numériques */
int decimal = 123;
int octal = 0777;
int hexadecimal = 0xABCD;

/* Tous les opérateurs */
void test_operators() {
    int a = 10, b = 5, result;
    
    /* Arithmétiques */
    result = a + b - c * d / e % f;
    
    /* Assignations */
    a = b;
    a += b; a -= b; a *= b; a /= b; a %= b;
    
    /* Comparaisons */
    result = a == b;
    result = a != b;
    result = a < b;
    result = a > b;
    result = a <= b;
    result = a >= b;
    
    /* Logiques */
    result = a && b;
    result = a || b;
    result = !a;
    
    /* Bitwise */
    result = a & b;
    result = a | b;
    result = a ^ b;
    result = ~a;
    result = a << 2;
    result = a >> 2;
    a &= b; a |= b; a ^= b;
    a <<= 2; a >>= 2;
    
    /* Incrémentation */
    ++a; a++; --b; b--;
    
    /* Ternaire */
    result = (a > b) ? a : b;
}

/* Pointeurs et tableaux */
void test_pointers() {
    int value = 42;
    int *ptr = &value;
    int deref = *ptr;
    
    int array[10];
    array[0] = 100;
    ptr = array;
}

/* Structures */
struct Point {
    int x, y;
};

union Data {
    int i;
    float f;
};

typedef struct {
    char name[20];
    int age;
} Person;

enum Status {
    ACTIVE,
    INACTIVE = 10,
    PENDING
};

/* Accès aux membres */
void test_members() {
    struct Point p;
    p.x = 10;
    p.y = 20;
    
    struct Point *ptr = &p;
    ptr->x = 30;
    ptr->y = 40;
}

/* Structures de contrôle */
void test_control() {
    int i, j;
    
    /* If/else */
    if (i > 0) {
        j = 1;
    } else if (i < 0) {
        j = -1;
    } else {
        j = 0;
    }
    
    /* Switch */
    switch (i) {
        case 1:
            break;
        case 2:
            continue;
        default:
            goto end;
    }
    
    /* While */
    while (i > 0) {
        i--;
    }
    
    /* Do-while */
    do {
        i++;
    } while (i < 10);
    
    /* For */
    for (j = 0; j < 10; j++) {
        if (j == 5) break;
        if (j == 7) continue;
    }
    
    end:
        return;
}

/* Classes de stockage et qualifieurs */
static int static_var = 100;
extern int extern_var;
const int const_var = 42;
volatile int volatile_var;

/* Caractères d'échappement de base */
void test_escapes() {
    char newline = '\n';
    char tab = '\t';
    char backslash = '\\';
    char quote = '\'';
    char dquote = '\"';
    char null_term = '\0';
}

/* Point d'entrée */
int main() {
    /* Initialisations */
    int numbers[] = {1, 2, 3, 4, 5};
    char text[] = "Hello World";
    
    struct Point origin = {0, 0};
    
    /* Sizeof */
    int size = sizeof(int);
    
    /* Appel fonction */
    printf("Test: %d %s\n", 42, "OK");
    
    return 0;
}
